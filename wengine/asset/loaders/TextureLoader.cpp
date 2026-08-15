#include "wengine/asset/loaders/TextureLoader.h"

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <stdexcept>
#include <vector>

void TextureData::free()
{
    if (pixels)
    {
        stbi_image_free(pixels);
        pixels = nullptr;
    }
    width = 0;
    height = 0;
    channels = 0;
}

TextureData TextureLoader::load(const std::string& path)
{
    return load(path, 4); // RGBA
}

TextureData TextureLoader::load(const std::string& path, int desiredChannels)
{
    // Flip vertically. OpenGL expects origin at bottom-left, images at top-left
    stbi_set_flip_vertically_on_load(true);

    TextureData data;

    data.pixels = stbi_load(
        path.c_str(),
        &data.width,
        &data.height,
        &data.channels,
        desiredChannels
    );

    if (desiredChannels != 0)
    {
        data.channels = desiredChannels;
    }

    if (!data.pixels)
    {
        std::string reason = stbi_failure_reason();
        spdlog::error("TextureLoader: failed to load '{}': {}", path, reason);
        throw std::runtime_error("TextureLoader: failed to load '" + path + "': " + reason);
    }

    spdlog::info("TextureLoader: loaded '{}' ({}x{}, {} channels)", 
                 path, data.width, data.height, data.channels);

    return data;
}

bool TextureLoader::isSupported(const std::string& path)
{
    // Find the file extension
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos)
        return false;
    
    std::string ext = path.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // List of all supported stb_image formats
    // Based on: https://github.com/nothings/stb/blob/master/stb_image.h
    static const std::vector<std::string> supportedExtensions = {
        "jpg",      // JPEG
        "jpeg",     // JPEG
        "jpe",      // JPEG
        "jfif",     // JPEG File Interchange Format
        "png",      // Portable Network Graphics
        "tga",      // Targa
        "icb",      // Targa (alternate extension)
        "vda",      // Targa (alternate extension)
        "vst",      // Targa (alternate extension)
        "bmp",      // Windows Bitmap
        "dib",      // Device Independent Bitmap
        "psd",      // Adobe Photoshop
        "gif",      // Graphics Interchange Format
        "hdr",      // Radiance RGBE
        "rgbe",     // Radiance RGBE (alternate extension)
        "pic",      // Softimage PIC
        "pnm",      // Portable Any Map
        "ppm",      // Portable Pixel Map (binary only)
        "pgm"       // Portable Gray Map (binary only)
    };
    
    return std::find(supportedExtensions.begin(), supportedExtensions.end(), ext) != supportedExtensions.end();
}
