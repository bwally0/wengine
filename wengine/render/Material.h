#pragma once

#include "wengine/render/Shader.h"
#include "wengine/render/Texture.h"

#include <memory>
#include <glm/glm.hpp>

class Material
{
public:
    Material() = default;
    ~Material() = default;

    Material(const Material&) = default;               // copyable
    Material& operator=(const Material&) = default;
    
    Material(Material&&) noexcept = default;           // movable
    Material& operator=(Material&&) noexcept = default;

    void bind() const;
    void unbind() const;
    
    bool isValid() const { return shader != nullptr; }

    // Shader
    std::shared_ptr<Shader> shader;
    
    // Texture maps
    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> normalTexture;
    std::shared_ptr<Texture> specularTexture;
    std::shared_ptr<Texture> emissiveTexture;
    
    // Material properties
    glm::vec3 color     = glm::vec3(1.0f);
    glm::vec3 emissive  = glm::vec3(0.0f);
    
    float metallic      = 0.0f;
    float roughness     = 0.5f;
    float opacity       = 1.0f;

    // Texture tiling and offset
    glm::vec2 textureScale  = glm::vec2(1.0f);
    glm::vec2 textureOffset = glm::vec2(0.0f);
};
