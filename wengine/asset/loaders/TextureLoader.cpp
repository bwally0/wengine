#include "wengine/asset/loaders/TextureLoader.h"

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <stdexcept>

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
    std::string ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return ext == "png"  || ext == "jpg"  || ext == "jpeg" ||
           ext == "tga"  || ext == "bmp"  || ext == "psd"  ||
           ext == "gif"  || ext == "hdr"  || ext == "pic"  ||
           ext == "pnm"  || ext == "pgm"  || ext == "ppm";
}
