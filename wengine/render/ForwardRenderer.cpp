#include "wengine/render/ForwardRenderer.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/scene/components/Transform.h"
#include "wengine/scene/components/Mesh.h"
#include "wengine/scene/components/Camera.h"
#include "wengine/core/Application.h"
#include "wengine/core/events/WindowResizeEvent.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

ForwardRenderer::ForwardRenderer(SceneModule& scene, EventBus& bus, const AppConfig& config)
    : m_scene(scene)
    , m_aspectRatio(static_cast<float>(config.width) / static_cast<float>(config.height))
{
    bus.subscribe<WindowResizeEvent>([this](const WindowResizeEvent& e) {
        m_aspectRatio = static_cast<float>(e.width) / static_cast<float>(e.height);
    });
}

void ForwardRenderer::render()
{
    auto& cameraPool = m_scene.query<Camera>();
    auto& cameras    = cameraPool.components();
    auto& cameraEntities = cameraPool.entities();

    Transform* cameraTransform = nullptr;
    Camera*    activeCamera    = nullptr;

    // pick active camera
    for (size_t i = 0; i < cameras.size(); i++)
    {
        Camera& camera = cameras[i];
        if (!camera.active) continue;

        if (!activeCamera || camera.priority > activeCamera->priority)
        {
            EntityID entity = cameraEntities[i];
            Transform* transform = m_scene.getComponent<Transform>(entity);
            if (transform)
            {
                activeCamera    = &camera;
                cameraTransform = transform;
            }
        }
    }

    if (!activeCamera || !cameraTransform) return;

    glm::mat4 view = glm::inverse(
        glm::translate(glm::mat4(1.0f), cameraTransform->position)
        * glm::mat4_cast(cameraTransform->rotation)
    );
    glm::mat4 projection = glm::perspective(
        glm::radians(activeCamera->fov),
        m_aspectRatio,
        activeCamera->nearPlane,
        activeCamera->farPlane
    );

    // render all mesh entities
    auto& meshPool = m_scene.query<Mesh>();
    auto& meshes   = meshPool.components();
    auto& entities = meshPool.entities();

    for (size_t i = 0; i < meshes.size(); i++)
    {
        Mesh&    mesh   = meshes[i];
        EntityID entity = entities[i];

        if (!mesh.vertexArray || !mesh.indexBuffer || !mesh.shader) continue;

        Transform* transform = m_scene.getComponent<Transform>(entity);
        if (!transform) continue;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform->position)
                        * glm::mat4_cast(transform->rotation)
                        * glm::scale(glm::mat4(1.0f), transform->scale);

        glm::mat4 mvp = projection * view * model;

        mesh.shader->bind();
        mesh.shader->setMat4("u_MVP", mvp);
        mesh.vertexArray->bind();
        glDrawElements(GL_TRIANGLES, mesh.indexBuffer->count(), GL_UNSIGNED_INT, nullptr);
    }
}
