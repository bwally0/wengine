#pragma once

#include <glm/glm.hpp>

struct SpotLightComponent
{
    glm::vec3 direction     = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color         = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity         = 1.0f; 
    bool enabled            = true;
    
    // Spotlight cone angles
    float innerCutoff       = glm::radians(12.5f); ///< Inner cone (full intensity)
    float outerCutoff       = glm::radians(17.5f); ///< Outer cone (smooth falloff to zero)
    
    // Attenuation parameters (for distance falloff)
    // Light intensity = 1.0 / (constant + linear * distance + quadratic * distance^2)
    float constant          = 1.0f;
    float linear            = 0.09f;
    float quadratic         = 0.032f;
};
