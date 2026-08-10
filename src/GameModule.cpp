#include <glad/glad.h>

#include "GameModule.h"
#include "wengine/core/ResourceRegistry.h"
#include "wengine/core/Window.h"

#include "wengine/scene/components/Transform.h"
#include "wengine/scene/components/Mesh.h"
#include "wengine/scene/components/Camera.h"
#include "wengine/render/Shader.h"
#include "wengine/render/VertexBuffer.h"
#include "wengine/render/IndexBuffer.h"
#include "wengine/render/VertexArray.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <spdlog/spdlog.h>

#include <cmath>

void GameModule::init(ResourceRegistry& resources)
{
    m_window = &resources.get<Window>();

    // hello triangle
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
    };

    uint32_t indices[] = { 0, 1, 2 };

    auto shader = std::make_shared<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    auto vbo    = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    auto ibo    = std::make_shared<IndexBuffer>(indices, 3);
    auto vao    = std::make_shared<VertexArray>();

    vao->addVertexBuffer(*vbo, {
        { 0, 3, GL_FLOAT, false, 6 * sizeof(float), 0 },
        { 1, 3, GL_FLOAT, false, 6 * sizeof(float), 3 * sizeof(float) },
    });
    vao->setIndexBuffer(*ibo);

    EntityID triangle = m_scene.createEntity();
    m_scene.addComponent<Transform>(triangle, Transform{
        .position = { 0.0f, 0.0f, -2.0f }
    });
    m_scene.addComponent<Mesh>(triangle, Mesh{
        .vertexArray = vao,
        .indexBuffer = ibo,
        .shader      = shader
    });

    spdlog::info("GameModule: created test triangle");

    // create camera entity
    m_cameraEntity = m_scene.createEntity();
    m_scene.addComponent<Transform>(m_cameraEntity, Transform{
        .position = { 0.0f, 0.0f, 2.0f }
    });
    m_scene.addComponent<Camera>(m_cameraEntity, Camera{
        .active = true
    });

    spdlog::info("GameModule: created free fly camera");
}

void GameModule::update(double deltaTime)
{
    float dt = static_cast<float>(deltaTime);

    if (m_input.wasKeyPressed(Key::Escape))
    {
        glfwSetWindowShouldClose(m_window->handle, GLFW_TRUE);
        return;
    }


    if (m_input.isMouseButtonDown(Mouse::Right))
    {
        float dx = static_cast<float>(m_input.mouseDeltaX()) * m_lookSensitivity;
        float dy = static_cast<float>(m_input.mouseDeltaY()) * m_lookSensitivity;

        m_yaw   += dx;
        m_pitch -= dy;
    }

    // clamp pitch
    if (m_pitch > 89.0f)  m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    float yawRad   = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    glm::vec3 forward;
    forward.x = std::cos(yawRad) * std::cos(pitchRad);
    forward.y = std::sin(pitchRad);
    forward.z = std::sin(yawRad) * std::cos(pitchRad);
    forward = glm::normalize(forward);

    glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    glm::vec3 right   = glm::normalize(glm::cross(forward, worldUp));
    glm::vec3 up      = glm::normalize(glm::cross(right, forward));

    Transform* transform = m_scene.getComponent<Transform>(m_cameraEntity);
    if (!transform) return;

    float speed = m_moveSpeed * dt;

    if (m_input.isKeyDown(Key::W)) transform->position += forward * speed;
    if (m_input.isKeyDown(Key::S)) transform->position -= forward * speed;
    if (m_input.isKeyDown(Key::A)) transform->position -= right * speed;
    if (m_input.isKeyDown(Key::D)) transform->position += right * speed;
    if (m_input.isKeyDown(Key::Q)) transform->position += worldUp * speed;
    if (m_input.isKeyDown(Key::E)) transform->position -= worldUp * speed;

    glm::mat3 rotMatrix(right, up, -forward);
    transform->rotation = glm::quat_cast(rotMatrix);
}
