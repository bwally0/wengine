#pragma once

#include <string>
#include <cstdint>

class Texture
{
public:
    explicit Texture(const std::string& path);
    ~Texture();

    Texture(const Texture&)            = delete;  // non-copyable
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;            // movable
    Texture& operator=(Texture&& other) noexcept;

    // bind to a texture slot (0-15), slot 0 by default
    void bind(uint32_t slot = 0) const;
    void unbind() const;

    int width()    const { return m_width; }
    int height()   const { return m_height; }
    int channels() const { return m_channels; }

private:
    uint32_t m_id       = 0;
    int      m_width    = 0;
    int      m_height   = 0;
    int      m_channels = 0;
};
