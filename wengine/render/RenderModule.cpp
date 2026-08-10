#include "wengine/render/RenderModule.h"

#include "wengine/scene/SceneModule.h"
#include "wengine/core/ResourceRegistry.h"
#include "wengine/core/EventBus.h"
#include "wengine/core/Application.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

RenderModule::RenderModule(SceneModule& scene)
    : m_scene(scene)
{
}

void RenderModule::init(ResourceRegistry& resources)
{
    m_eventBus = &resources.get<EventBus>();
    m_config   = &resources.get<AppConfig>();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    m_forwardRenderer = std::make_unique<ForwardRenderer>(m_scene, *m_eventBus, *m_config);

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
