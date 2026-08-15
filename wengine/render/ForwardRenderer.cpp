#include "wengine/render/ForwardRenderer.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/scene/components/TransformComponent.h"
#include "wengine/scene/components/MeshComponent.h"
#include "wengine/scene/components/RenderComponent.h"
#include "wengine/scene/components/CameraComponent.h"
#include "wengine/scene/components/DirectionalLightComponent.h"
#include "wengine/scene/components/PointLightComponent.h"
#include "wengine/scene/components/SpotLightComponent.h"
#include "wengine/core/Application.h"
#include "wengine/core/events/WindowResizeEvent.h"
#include "wengine/render/Mesh.h"
#include "wengine/render/Material.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

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

    glm::vec3 cameraPos = cameraTransform->position;
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

    
    // Directional lights
    auto& dirLightPool = m_scene.query<DirectionalLightComponent>();
    auto& dirLights = dirLightPool.components();
    
    // Point lights
    auto& pointLightPool = m_scene.query<PointLightComponent>();
    auto& pointLights = pointLightPool.components();
    auto& pointLightEntities = pointLightPool.entities();
    
    // Spot lights
    auto& spotLightPool = m_scene.query<SpotLightComponent>();
    auto& spotLights = spotLightPool.components();
    auto& spotLightEntities = spotLightPool.entities();
    
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

        // Bind material (shader + textures + material properties)
        renderComp->material->bind();
        
        auto shader = renderComp->material->shader;
        shader->setMat4("u_MVP", mvp);
        shader->setMat4("u_Model", model);
        shader->setVec3("u_CameraPos", cameraPos);
        
        // Set ambient lighting (simple global ambient)
        shader->setVec3("u_AmbientColor", glm::vec3(0.03f, 0.03f, 0.03f));
        
        int numDirLights = std::min(static_cast<int>(dirLights.size()), 4); // MAX_DIRECTIONAL_LIGHTS
        shader->setInt("u_NumDirectionalLights", numDirLights);
        
        for (int j = 0; j < numDirLights; j++)
        {
            const auto& light = dirLights[j];
            if (!light.enabled) continue;
            
            std::string base = "u_DirectionalLights[" + std::to_string(j) + "]";
            shader->setVec3(base + ".direction", light.direction);
            shader->setVec3(base + ".color", light.color);
            shader->setFloat(base + ".intensity", light.intensity);
        }
        
        int numPointLights = std::min(static_cast<int>(pointLights.size()), 16); // MAX_POINT_LIGHTS
        shader->setInt("u_NumPointLights", numPointLights);
        
        for (int j = 0; j < numPointLights; j++)
        {
            const auto& light = pointLights[j];
            if (!light.enabled) continue;
            
            // Get position from transform
            EntityID lightEntity = pointLightEntities[j];
            TransformComponent* lightTransform = m_scene.getComponent<TransformComponent>(lightEntity);
            glm::vec3 position = lightTransform ? lightTransform->position : glm::vec3(0.0f);
            
            std::string base = "u_PointLights[" + std::to_string(j) + "]";
            shader->setVec3(base + ".position", position);
            shader->setVec3(base + ".color", light.color);
            shader->setFloat(base + ".intensity", light.intensity);
            shader->setFloat(base + ".constant", light.constant);
            shader->setFloat(base + ".linear", light.linear);
            shader->setFloat(base + ".quadratic", light.quadratic);
        }
        
        int numSpotLights = std::min(static_cast<int>(spotLights.size()), 8); // MAX_SPOT_LIGHTS
        shader->setInt("u_NumSpotLights", numSpotLights);
        
        for (int j = 0; j < numSpotLights; j++)
        {
            const auto& light = spotLights[j];
            if (!light.enabled) continue;
            
            // Get position from transform
            EntityID lightEntity = spotLightEntities[j];
            TransformComponent* lightTransform = m_scene.getComponent<TransformComponent>(lightEntity);
            glm::vec3 position = lightTransform ? lightTransform->position : glm::vec3(0.0f);
            
            std::string base = "u_SpotLights[" + std::to_string(j) + "]";
            shader->setVec3(base + ".position", position);
            shader->setVec3(base + ".direction", light.direction);
            shader->setVec3(base + ".color", light.color);
            shader->setFloat(base + ".intensity", light.intensity);
            shader->setFloat(base + ".innerCutoff", cos(light.innerCutoff)); // cos for optimization
            shader->setFloat(base + ".outerCutoff", cos(light.outerCutoff));
            shader->setFloat(base + ".constant", light.constant);
            shader->setFloat(base + ".linear", light.linear);
            shader->setFloat(base + ".quadratic", light.quadratic);
        }

        // Draw mesh
        meshComp.mesh->bind();
        glDrawElements(GL_TRIANGLES, meshComp.mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
}
