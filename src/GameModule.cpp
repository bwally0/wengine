#include <glad/glad.h>

#include "GameModule.h"
#include "wengine/core/ResourceRegistry.h"
#include "wengine/core/Window.h"
#include "wengine/asset/AssetRegistry.h"

#include "wengine/scene/components/TransformComponent.h"
#include "wengine/scene/components/MeshComponent.h"
#include "wengine/scene/components/RenderComponent.h"
#include "wengine/scene/components/CameraComponent.h"
#include "wengine/render/Shader.h"
#include "wengine/render/Material.h"
#include "wengine/render/Mesh.h"
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
    auto& assets = resources.get<AssetRegistry>();

    // Load crate mesh and texture using AssetRegistry
    auto crateMesh = assets.loadMesh("assets/models/crate.fbx");
    if (!crateMesh)
    {
        spdlog::error("GameModule: Failed to load crate mesh");
        return;
    }

    // Create material with textured shader and diffuse texture
    auto crateMaterial = assets.createMaterial(
        "assets/shaders/textured.vert",
        "assets/shaders/textured.frag",
        "assets/textures/crate_diffuse.png"
    );
    
    if (!crateMaterial)
    {
        spdlog::error("GameModule: Failed to create crate material");
        return;
    }

    // Create crate entity
    EntityID crate = m_scene.createEntity();
    
    m_scene.addComponent<TransformComponent>(crate, TransformComponent{
        .position = {0.0f, 0.0f, -3.0f},
        .rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        .scale = {0.05f, 0.05f, 0.05f}
    });
    
    m_scene.addComponent<MeshComponent>(crate, MeshComponent{
        .mesh = crateMesh
    });
    
    m_scene.addComponent<RenderComponent>(crate, RenderComponent{
        .material = crateMaterial
    });

    spdlog::info("GameModule: created textured crate");

    // Log asset registry statistics
    assets.logStatistics();

    // create camera entity
    m_cameraEntity = m_scene.createEntity();
    m_scene.addComponent<TransformComponent>(m_cameraEntity, TransformComponent{
        .position = { 0.0f, 0.0f, 2.0f }
    });
    m_scene.addComponent<CameraComponent>(m_cameraEntity, CameraComponent{
        .active = true
    });
    m_input.setCursorMode(CursorMode::Disabled);

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

    CameraComponent* camera = m_scene.getComponent<CameraComponent>(m_cameraEntity);

    float dx = static_cast<float>(m_input.mouseDeltaX()) * m_lookSensitivity;
    float dy = static_cast<float>(m_input.mouseDeltaY()) * m_lookSensitivity;

    m_yaw   += dx;
    m_pitch -= dy;

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

    TransformComponent* transform = m_scene.getComponent<TransformComponent>(m_cameraEntity);
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
