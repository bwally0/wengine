#pragma once

#include <string>
#include <cstdint>

struct TextureData
{
    uint8_t* pixels   = nullptr;
    int      width    = 0;
    int      height   = 0;
    int      channels = 0;

    TextureData() = default;
    
    TextureData(TextureData&& texture) noexcept
        : pixels(texture.pixels)
        , width(texture.width)
        , height(texture.height)
        , channels(texture.channels)
    {
        texture.pixels = nullptr;
    }

    TextureData& operator=(TextureData&& texture) noexcept
    {
        if (this != &texture)
        {
            free();
            pixels = texture.pixels;
            width = texture.width;
            height = texture.height;
            channels = texture.channels;
            texture.pixels = nullptr;
        }
        return *this;
    }

    TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;

    ~TextureData() { free(); }

    void free();
    bool isValid() const { return pixels != nullptr; }
};

class TextureLoader
{
public:
    static TextureData load(const std::string& path);
    static TextureData load(const std::string& path, int desiredChannels);

    static bool isSupported(const std::string& path);

private:
    TextureLoader() = delete; // Static utility class
};
