#pragma once

#include <string>
#include <cstdint>

/**
 * @struct TextureData
 * @brief Represents raw texture data.
 * 
 * Used as an intermediate representation before uploading to the GPU.
 */
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

/**
 * @class TextureLoader
 * @brief Utility class for loading texture data from various file formats.
 * 
 * Uses the stb_image library to load textures and convert them into a TextureData structure.
 */
class TextureLoader
{
public:
    /**
     * @brief Loads texture data from a file.
     * 
     * Default channel count is 4 (RGBA).
     * 
     * @param path The file path to load the texture from.
     * @return TextureData The loaded texture data.
     */
    static TextureData load(const std::string& path);

    /**
     * @brief Loads texture data from a file with a desired number of channels.
     * 
     * @param path The file path to load the texture from.
     * @param desiredChannels The desired number of channels (e.g., 3 for RGB, 4 for RGBA).
     * @return TextureData The loaded texture data.
     */
    static TextureData load(const std::string& path, int desiredChannels);

    /**
     * @brief Checks if the file format of the given path is supported for texture loading.
     * 
     * @param path The file path to check.
     * @return true If the file format is supported.
     * @return false If the file format is not supported.
     */
    static bool isSupported(const std::string& path);

private:
    TextureLoader() = delete; // Static utility class
};
