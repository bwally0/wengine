#pragma once

#include "wengine/core/EventBus.h"
#include "wengine/scene/SceneModule.h"
#include <glm/glm.hpp>

struct AppConfig;

class ForwardRenderer
{
public:
    ForwardRenderer(SceneModule& scene, EventBus& bus, const AppConfig& config);

    void render();

private:
    SceneModule& m_scene;
    float        m_aspectRatio;
};
