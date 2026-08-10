#pragma once

#include "wengine/core/IModule.h"
#include "wengine/scene/SceneModule.h"
#include "wengine/render/RenderModule.h"
#include <string>
#include <vector>

class GameModule : public IModule
{
public:
    explicit GameModule(SceneModule& scene, RenderModule& render) 
        : m_scene(scene), m_render(render) {}

    void init(ResourceRegistry& resources) override;
    void update(double deltaTime)          override {}
    void render()                          override {}
    void shutdown()                        override {}

    std::vector<std::string> getDependencies() override
    {
        return { "SceneModule", "RenderModule" };
    }

private:
    SceneModule& m_scene;
    RenderModule& m_render;
};
