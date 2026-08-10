#pragma once

#include "wengine/core/IModule.h"
#include "wengine/core/InputModule.h"
#include "wengine/scene/SceneModule.h"
#include "wengine/scene/EntityManager.h"
#include "wengine/render/RenderModule.h"
#include <string>
#include <vector>

class GameModule : public IModule
{
public:
    explicit GameModule(SceneModule& scene, RenderModule& render, InputModule& input) 
        : m_scene(scene), m_render(render), m_input(input) {}

    void init(ResourceRegistry& resources) override;
    void update(double deltaTime)          override;
    void render()                          override {}
    void shutdown()                        override {}

    std::vector<std::string> getDependencies() override
    {
        return { "SceneModule", "RenderModule", "InputModule" };
    }

private:
    SceneModule&  m_scene;
    RenderModule& m_render;
    InputModule&  m_input;

    struct Window* m_window = nullptr;
    EntityID m_cameraEntity = 0;

    float m_moveSpeed = 5.0f;
    float m_lookSensitivity = 0.2f;
    float m_yaw   = -90.0f; // facing -Z
    float m_pitch = 0.0f;
};
