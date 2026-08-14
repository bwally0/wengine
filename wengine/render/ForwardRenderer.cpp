#include "wengine/render/ForwardRenderer.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/scene/components/TransformComponent.h"
#include "wengine/scene/components/MeshComponent.h"
#include "wengine/scene/components/RenderComponent.h"
#include "wengine/scene/components/CameraComponent.h"
#include "wengine/core/Application.h"
#include "wengine/core/events/WindowResizeEvent.h"
#include "wengine/render/Mesh.h"
#include "wengine/render/Material.h"

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
    auto& cameraPool = m_scene.query<CameraComponent>();
    auto& cameras    = cameraPool.components();
    auto& cameraEntities = cameraPool.entities();

    TransformComponent* cameraTransform = nullptr;
    CameraComponent*    activeCamera    = nullptr;

    // pick active camera
    for (size_t i = 0; i < cameras.size(); i++)
    {
        CameraComponent& camera = cameras[i];
        if (!camera.active) continue;

        if (!activeCamera || camera.priority > activeCamera->priority)
        {
            EntityID entity = cameraEntities[i];
            TransformComponent* transform = m_scene.getComponent<TransformComponent>(entity);
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

    // render all entities with MeshComponent
    auto& meshPool = m_scene.query<MeshComponent>();
    auto& meshComponents = meshPool.components();
    auto& entities = meshPool.entities();

    for (size_t i = 0; i < meshComponents.size(); i++)
    {
        MeshComponent& meshComp = meshComponents[i];
        EntityID entity = entities[i];

        // Must have RenderComponent
        RenderComponent* renderComp = m_scene.getComponent<RenderComponent>(entity);
        if (!renderComp || !renderComp->material) continue;

        // Must have TransformComponent
        TransformComponent* transform = m_scene.getComponent<TransformComponent>(entity);
        if (!transform) continue;

        // Validate resources
        if (!meshComp.mesh || !renderComp->material->shader) continue;

        // Build model matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform->position)
                        * glm::mat4_cast(transform->rotation)
                        * glm::scale(glm::mat4(1.0f), transform->scale);

        glm::mat4 mvp = projection * view * model;

        // Bind material (shader + textures)
        renderComp->material->bind();
        renderComp->material->shader->setMat4("u_MVP", mvp);
        renderComp->material->shader->setMat4("u_Model", model);

        // Draw mesh
        meshComp.mesh->bind();
        glDrawElements(GL_TRIANGLES, meshComp.mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
}
