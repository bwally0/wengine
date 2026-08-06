#pragma once

#include "wengine/core/EventBus.h"
#include "wengine/scene/SceneModule.h"
#include <glm/glm.hpp>

// ForwardRenderer is owned and driven by RenderModule.
// It iterates (Transform + Mesh) entities and submits draw calls.
class ForwardRenderer
{
public:
    ForwardRenderer(SceneModule& scene, const glm::mat4& projection, EventBus& bus);

    void render();

    void setView(const glm::mat4& view);

private:
    SceneModule& m_scene;
    glm::mat4    m_projection;
    glm::mat4    m_view = glm::mat4(1.0f);
};
