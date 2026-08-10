#include "wengine/render/RenderModule.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/core/ResourceRegistry.h"
#include "wengine/core/EventBus.h"
#include "wengine/core/Application.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

RenderModule::RenderModule(SceneModule& scene)
    : m_scene(scene)
    , m_projection(glm::mat4(1.0f))
{
}

void RenderModule::init(ResourceRegistry& resources)
{
    m_eventBus = &resources.get<EventBus>();
    m_config   = &resources.get<AppConfig>();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    updateProjection(m_config->width, m_config->height);

    m_forwardRenderer = std::make_unique<ForwardRenderer>(m_scene, m_projection, *m_eventBus);

    spdlog::info("RenderModule: initialized");
}

void RenderModule::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_forwardRenderer->render();
}

void RenderModule::shutdown()
{
    spdlog::info("RenderModule: shutdown");
}

void RenderModule::updateProjection(int width, int height)
{
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    m_projection = glm::perspective(glm::radians(m_fov), aspect, m_nearPlane, m_farPlane);
}
