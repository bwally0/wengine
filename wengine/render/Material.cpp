#include "wengine/render/Material.h"

#include <spdlog/spdlog.h>

void Material::bind() const
{
    if (!shader)
    {
        spdlog::warn("Material::bind: No shader set");
        return;
    }
    
    // Bind shader
    shader->bind();
    
    // Bind textures to their respective slots
    int textureSlot = 0;
    
    if (diffuseTexture)
    {
        diffuseTexture->bind(textureSlot);
        shader->setInt("u_DiffuseTexture", textureSlot);
        shader->setInt("u_HasDiffuseTexture", 1);
        textureSlot++;
    }
    else
    {
        shader->setInt("u_HasDiffuseTexture", 0);
    }
    
    if (normalTexture)
    {
        normalTexture->bind(textureSlot);
        shader->setInt("u_NormalTexture", textureSlot);
        shader->setInt("u_HasNormalTexture", 1);
        textureSlot++;
    }
    else
    {
        shader->setInt("u_HasNormalTexture", 0);
    }
    
    if (specularTexture)
    {
        specularTexture->bind(textureSlot);
        shader->setInt("u_SpecularTexture", textureSlot);
        shader->setInt("u_HasSpecularTexture", 1);
        textureSlot++;
    }
    else
    {
        shader->setInt("u_HasSpecularTexture", 0);
    }
    
    if (emissiveTexture)
    {
        emissiveTexture->bind(textureSlot);
        shader->setInt("u_EmissiveTexture", textureSlot);
        shader->setInt("u_HasEmissiveTexture", 1);
        textureSlot++;
    }
    else
    {
        shader->setInt("u_HasEmissiveTexture", 0);
    }
    
    // Set material properties as uniforms
    shader->setVec3("u_Color", color);
    shader->setVec3("u_Emissive", emissive);
    shader->setFloat("u_Metallic", metallic);
    shader->setFloat("u_Roughness", roughness);
    shader->setFloat("u_Opacity", opacity);
    
    // Set texture transforms
    shader->setVec2("u_TextureScale", textureScale);
    shader->setVec2("u_TextureOffset", textureOffset);
}

void Material::unbind() const
{
    if (shader)
    {
        shader->unbind();
    }
}
