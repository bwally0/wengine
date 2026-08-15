#pragma once

#include <glm/glm.hpp>

struct PointLightComponent
{
    glm::vec3 color     = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity     = 1.0f;
    bool enabled        = true;
    
    // Attenuation parameters (for distance falloff)
    // Light intensity = 1.0 / (constant + linear * distance + quadratic * distance^2)
    float constant      = 1.0f;
    float linear        = 0.09f;
    float quadratic     = 0.032f;
};
