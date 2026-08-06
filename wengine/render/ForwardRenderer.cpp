#include "wengine/render/ForwardRenderer.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/scene/components/Transform.h"
#include "wengine/scene/components/Mesh.h"
#include "wengine/core/events/WindowResizeEvent.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

ForwardRenderer::ForwardRenderer(SceneModule& scene, const glm::mat4& projection, EventBus& bus)
    : m_scene(scene)
    , m_projection(projection)
    , m_view(glm::mat4(1.0f))
{
    bus.subscribe<WindowResizeEvent>([this](const WindowResizeEvent& e) {
        float aspect = static_cast<float>(e.width) / static_cast<float>(e.height);
        m_projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    });
}

void ForwardRenderer::render()
{
    auto& meshPool = m_scene.query<Mesh>();

    auto& meshes   = meshPool.components();
    auto& entities = meshPool.entities();

    for (size_t i = 0; i < meshes.size(); i++)
    {
        Mesh&      mesh   = meshes[i];
        EntityID   entity = entities[i];

        if (!mesh.vertexArray || !mesh.indexBuffer || !mesh.shader) continue;

        Transform* transform = m_scene.getComponent<Transform>(entity);
        if (!transform) continue;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform->position)
                        * glm::mat4_cast(transform->rotation)
                        * glm::scale(glm::mat4(1.0f), transform->scale);

        glm::mat4 mvp = m_projection * m_view * model;

        mesh.shader->bind();
        mesh.shader->setMat4("u_MVP", mvp);
        mesh.vertexArray->bind();
        glDrawElements(GL_TRIANGLES, mesh.indexBuffer->count(), GL_UNSIGNED_INT, nullptr);
    }
}

void ForwardRenderer::setView(const glm::mat4& view)
{
    m_view = view;
}
