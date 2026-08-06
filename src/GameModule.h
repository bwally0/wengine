#pragma once

#include "wengine/core/IModule.h"
#include "wengine/scene/SceneModule.h"
#include <string>
#include <vector>

class GameModule : public IModule
{
public:
    explicit GameModule(SceneModule& scene) : m_scene(scene) {}

    void init()                   override;
    void update(double deltaTime) override {}
    void render()                 override {}
    void shutdown()               override {}

    std::vector<std::string> getDependencies() override
    {
        return { "SceneModule", "RenderModule" };
    }

private:
    SceneModule& m_scene;
};
