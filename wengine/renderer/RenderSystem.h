#pragma once

#include "wengine/core/ISystem.h"
#include "wengine/scene/SceneModule.h"
#include <glm/glm.hpp>

class RenderSystem : public ISystem
{
public:
    RenderSystem(SceneModule& scene, const glm::mat4& projection);

    void update(double deltaTime) override;

    void setView(const glm::mat4& view);
    void setProjection(const glm::mat4& projection);

private:
    SceneModule& m_scene;
    glm::mat4 m_projection;
    glm::mat4 m_view = glm::mat4(1.0f);
};