#include "wengine/renderer/Texture.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& path)
{
    // flip vertically, OpenGL expects origin at bottom-left, images at top-left
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data)
    {
        throw std::runtime_error("Texture: failed to load '" + path + "': " + stbi_failure_reason());
    }

    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;

    if (m_channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat     = GL_RGBA;
    }
    else if (m_channels == 1)
    {
        internalFormat = GL_R8;
        dataFormat     = GL_RED;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    spdlog::info("Texture: loaded '{}' ({}x{}, {} channels)", path, m_width, m_height, m_channels);
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
