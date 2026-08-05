#include "wengine/renderer/RenderSystem.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/scene/components/Transform.h"
#include "wengine/scene/components/Mesh.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

RenderSystem::RenderSystem(SceneModule& scene, const glm::mat4& projection)
    : m_scene(scene)
    , m_projection(projection)
    , m_view(glm::mat4(1.0f))
{
}

void RenderSystem::update(double /*deltaTime*/)
{
    auto& meshPool = m_scene.query<Mesh>();

    auto& meshes   = meshPool.components();
    auto& entities = meshPool.entities();

    for (size_t i = 0; i < meshes.size(); i++)
    {
        Mesh&      mesh   = meshes[i];
        EntityID   entity = entities[i];

        // skip mesh if missing elements
        if (!mesh.vertexArray || !mesh.indexBuffer || !mesh.shader) continue;

        Transform* transform = m_scene.getComponent<Transform>(entity); 
        if (!transform) continue;

        // build model matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform->position)
                        * glm::mat4_cast(transform->rotation)
                        * glm::scale(glm::mat4(1.0f), transform->scale);

        // build mvp matrix
        glm::mat4 mvp = m_projection * m_view * model;

        mesh.shader->bind();
        mesh.shader->setMat4("u_MVP", mvp);

        mesh.vertexArray->bind();
        glDrawElements(GL_TRIANGLES, mesh.indexBuffer->count(), GL_UNSIGNED_INT, nullptr);
    }
}

void RenderSystem::setView(const glm::mat4& view)
{
    m_view = view;
}

void RenderSystem::setProjection(const glm::mat4& projection)
{
    m_projection = projection;
}
