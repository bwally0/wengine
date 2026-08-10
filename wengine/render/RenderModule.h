#pragma once

#include "wengine/core/IModule.h"
#include "wengine/render/ForwardRenderer.h"

#include <memory>
#include <string>
#include <vector>

class SceneModule;
class EventBus;
struct AppConfig;

class RenderModule : public IModule
{
public:
    explicit RenderModule(SceneModule& scene);

    void init(ResourceRegistry& resources) override;
    void update(double deltaTime)           override {}
    void render()                           override;
    void shutdown()                         override;

    std::vector<std::string> getDependencies() override
    {
        return { "SceneModule" };
    }

private:
    SceneModule&                     m_scene;
    EventBus*                        m_eventBus = nullptr;
    AppConfig*                       m_config   = nullptr;
    std::unique_ptr<ForwardRenderer> m_forwardRenderer;
};
