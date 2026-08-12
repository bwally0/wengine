#include "wengine/render/Texture.h"
#include "wengine/asset/loaders/TextureLoader.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

std::shared_ptr<Texture> Texture::create(const std::string& path)
{
    TextureData data = TextureLoader::load(path);
    return create(data);
}

std::shared_ptr<Texture> Texture::create(const TextureData& data)
{
    if (!data.isValid())
    {
        throw std::runtime_error("Texture::create: invalid TextureData");
    }
    
    auto texture = std::shared_ptr<Texture>(new Texture());
    texture->m_width = data.width;
    texture->m_height = data.height;
    texture->m_channels = data.channels;
    
    // Determine OpenGL format based on channels
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat = GL_RGB;
    
    if (data.channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (data.channels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }
    else if (data.channels == 1)
    {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    }
    else if (data.channels == 2)
    {
        internalFormat = GL_RG8;
        dataFormat = GL_RG;
    }
    
    // Create and configure OpenGL texture
    glGenTextures(1, &texture->m_id);
    glBindTexture(GL_TEXTURE_2D, texture->m_id);
    
    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload pixel data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, data.width, data.height, 
                 0, dataFormat, GL_UNSIGNED_BYTE, data.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    spdlog::info("Texture: created ({}x{}, {} channels)", data.width, data.height, data.channels);
    
    return texture;
}

Texture::~Texture()
{
    if (m_id) glDeleteTextures(1, &m_id);
}

// move object
Texture::Texture(Texture&& other) noexcept
    : m_id(other.m_id), m_width(other.m_width), m_height(other.m_height), m_channels(other.m_channels)
{
    other.m_id = 0;
}

// move object
Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        if (m_id) glDeleteTextures(1, &m_id);
        m_id          = other.m_id;
        m_width       = other.m_width;
        m_height      = other.m_height;
        m_channels    = other.m_channels;
        other.m_id    = 0;
    }
    return *this;
}

void Texture::bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
