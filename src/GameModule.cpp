#include "GameModule.h"

#include "wengine/scene/components/Transform.h"
#include "wengine/scene/components/Mesh.h"
#include "wengine/render/Shader.h"
#include "wengine/render/VertexBuffer.h"
#include "wengine/render/IndexBuffer.h"
#include "wengine/render/VertexArray.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

void GameModule::init()
{
    // vertex data: position (xyz) + color (rgb)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // bottom-left, red
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom-right, green
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // top-center, blue
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
}
