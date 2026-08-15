#include "wengine/asset/AssetRegistry.h"

#include "wengine/render/Mesh.h"
#include "wengine/render/Texture.h"
#include "wengine/render/Shader.h"
#include "wengine/render/Material.h"

#include <spdlog/spdlog.h>
#include <algorithm>
#include <cctype>

std::shared_ptr<Mesh> AssetRegistry::loadMesh(const std::string& path)
{
    std::string key = normalizePath(path);
    
    std::lock_guard<std::mutex> lock(m_meshMutex);
    
    // Check cache first
    if (auto cached = getCached(m_meshCache, m_meshMutex, key))
    {
        return cached;
    }
    
    // Not cached, load from disk
    spdlog::info("AssetRegistry: Loading mesh '{}'", path);
    
    auto mesh = Mesh::create(path);
    if (!mesh)
    {
        spdlog::error("AssetRegistry: Failed to load mesh '{}'", path);
        return nullptr;
    }
    
    // Store weak_ptr in cache
    insertCache(m_meshCache, key, mesh);
    
    // Cleanup expired entries if cache is large
    if (m_meshCache.size() > CLEANUP_THRESHOLD)
    {
        spdlog::debug("AssetRegistry: Mesh cache exceeded threshold, cleaning up expired entries");
        cleanupCache(m_meshCache);
    }
    
    return mesh;
}

std::shared_ptr<Texture> AssetRegistry::loadTexture(const std::string& path)
{
    std::string key = normalizePath(path);
    
    std::lock_guard<std::mutex> lock(m_textureMutex);
    
    // Check cache first
    if (auto cached = getCached(m_textureCache, m_textureMutex, key))
    {
        return cached;
    }
    
    // Not cached, load from disk
    spdlog::info("AssetRegistry: Loading texture '{}'", path);
    
    auto texture = Texture::create(path);
    if (!texture)
    {
        spdlog::error("AssetRegistry: Failed to load texture '{}'", path);
        return nullptr;
    }
    
    // Store weak_ptr in cache
    insertCache(m_textureCache, key, texture);
    
    // Cleanup expired entries if cache is large
    if (m_textureCache.size() > CLEANUP_THRESHOLD)
    {
        spdlog::debug("AssetRegistry: Texture cache exceeded threshold, cleaning up expired entries");
        cleanupCache(m_textureCache);
    }
    
    return texture;
}

std::shared_ptr<Shader> AssetRegistry::loadShader(
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    // Composite key: "vert.glsl|frag.glsl"
    std::string key = normalizePath(vertexPath) + "|" + normalizePath(fragmentPath);
    
    std::lock_guard<std::mutex> lock(m_shaderMutex);
    
    // Check cache first
    if (auto cached = getCached(m_shaderCache, m_shaderMutex, key))
    {
        return cached;
    }
    
    // Not cached, load from disk
    spdlog::info("AssetRegistry: Loading shader '{}' + '{}'", vertexPath, fragmentPath);
    
    try
    {
        // Note: Shader constructor takes paths directly
        auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
        
        // Store weak_ptr in cache
        insertCache(m_shaderCache, key, shader);
        
        // Cleanup expired entries if cache is large
        if (m_shaderCache.size() > CLEANUP_THRESHOLD)
        {
            spdlog::debug("AssetRegistry: Shader cache exceeded threshold, cleaning up expired entries");
            cleanupCache(m_shaderCache);
        }
        
        return shader;
    }
    catch (const std::exception& e)
    {
        spdlog::error("AssetRegistry: Failed to load shader '{}' + '{}': {}", 
                      vertexPath, fragmentPath, e.what());
        return nullptr;
    }
}

std::shared_ptr<Material> AssetRegistry::createMaterial(
    const std::string& shaderVert,
    const std::string& shaderFrag,
    const std::string& diffusePath,
    const std::string& normalPath,
    const std::string& specularPath,
    const std::string& emissivePath)
{
    // Load shader (cached)
    auto shader = loadShader(shaderVert, shaderFrag);
    if (!shader)
    {
        spdlog::error("AssetRegistry: Cannot create material without valid shader");
        return nullptr;
    }
    
    // Create material
    auto material = std::make_shared<Material>();
    material->shader = shader;
    
    // Load diffuse texture if provided (cached)
    if (!diffusePath.empty())
    {
        material->diffuseTexture = loadTexture(diffusePath);
        if (!material->diffuseTexture)
        {
            spdlog::warn("AssetRegistry: Failed to load diffuse texture '{}' for material", 
                         diffusePath);
        }
    }
    
    // Load normal texture if provided (cached)
    if (!normalPath.empty())
    {
        material->normalTexture = loadTexture(normalPath);
        if (!material->normalTexture)
        {
            spdlog::warn("AssetRegistry: Failed to load normal texture '{}' for material", 
                         normalPath);
        }
    }
    
    // Load specular texture if provided (cached)
    if (!specularPath.empty())
    {
        material->specularTexture = loadTexture(specularPath);
        if (!material->specularTexture)
        {
            spdlog::warn("AssetRegistry: Failed to load specular texture '{}' for material", 
                         specularPath);
        }
    }
    
    // Load emissive texture if provided (cached)
    if (!emissivePath.empty())
    {
        material->emissiveTexture = loadTexture(emissivePath);
        if (!material->emissiveTexture)
        {
            spdlog::warn("AssetRegistry: Failed to load emissive texture '{}' for material", 
                         emissivePath);
        }
    }
    
    spdlog::debug("AssetRegistry: Created material with shader '{}' + '{}'", 
                  shaderVert, shaderFrag);
    
    return material;
}

//=============================================================================
// QUERY API
//=============================================================================

bool AssetRegistry::hasMesh(const std::string& path) const
{
    std::string key = normalizePath(path);
    std::lock_guard<std::mutex> lock(m_meshMutex);
    
    auto it = m_meshCache.find(key);
    if (it != m_meshCache.end())
    {
        return !it->second.expired();
    }
    return false;
}

bool AssetRegistry::hasTexture(const std::string& path) const
{
    std::string key = normalizePath(path);
    std::lock_guard<std::mutex> lock(m_textureMutex);
    
    auto it = m_textureCache.find(key);
    if (it != m_textureCache.end())
    {
        return !it->second.expired();
    }
    return false;
}

bool AssetRegistry::hasShader(const std::string& vertexPath, const std::string& fragmentPath) const
{
    std::string key = normalizePath(vertexPath) + "|" + normalizePath(fragmentPath);
    std::lock_guard<std::mutex> lock(m_shaderMutex);
    
    auto it = m_shaderCache.find(key);
    if (it != m_shaderCache.end())
    {
        return !it->second.expired();
    }
    return false;
}

size_t AssetRegistry::getMeshCacheSize() const
{
    std::lock_guard<std::mutex> lock(m_meshMutex);
    return m_meshCache.size();
}

size_t AssetRegistry::getTextureCacheSize() const
{
    std::lock_guard<std::mutex> lock(m_textureMutex);
    return m_textureCache.size();
}

size_t AssetRegistry::getShaderCacheSize() const
{
    std::lock_guard<std::mutex> lock(m_shaderMutex);
    return m_shaderCache.size();
}

size_t AssetRegistry::getActiveAssetCount() const
{
    std::lock_guard<std::mutex> meshLock(m_meshMutex);
    std::lock_guard<std::mutex> textureLock(m_textureMutex);
    std::lock_guard<std::mutex> shaderLock(m_shaderMutex);
    
    return countActive(m_meshCache) + 
           countActive(m_textureCache) + 
           countActive(m_shaderCache);
}

void AssetRegistry::cleanupExpired()
{
    {
        std::lock_guard<std::mutex> lock(m_meshMutex);
        cleanupCache(m_meshCache);
    }
    
    {
        std::lock_guard<std::mutex> lock(m_textureMutex);
        cleanupCache(m_textureCache);
    }
    
    {
        std::lock_guard<std::mutex> lock(m_shaderMutex);
        cleanupCache(m_shaderCache);
    }
    
    spdlog::debug("AssetRegistry: Cleaned up expired cache entries");
}

void AssetRegistry::clear()
{
    {
        std::lock_guard<std::mutex> lock(m_meshMutex);
        m_meshCache.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(m_textureMutex);
        m_textureCache.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(m_shaderMutex);
        m_shaderCache.clear();
    }
    
    spdlog::info("AssetRegistry: Cleared all asset caches");
}

void AssetRegistry::logStatistics() const
{
    std::lock_guard<std::mutex> meshLock(m_meshMutex);
    std::lock_guard<std::mutex> textureLock(m_textureMutex);
    std::lock_guard<std::mutex> shaderLock(m_shaderMutex);
    
    size_t activeMeshes    = countActive(m_meshCache);
    size_t activeTextures  = countActive(m_textureCache);
    size_t activeShaders   = countActive(m_shaderCache);
    
    size_t totalMeshes     = m_meshCache.size();
    size_t totalTextures   = m_textureCache.size();
    size_t totalShaders    = m_shaderCache.size();

    spdlog::info("=== AssetRegistry Statistics ===");
    spdlog::info("Meshes:   {} active / {} cached", activeMeshes, totalMeshes);
    spdlog::info("Textures: {} active / {} cached", activeTextures, totalTextures);
    spdlog::info("Shaders:  {} active / {} cached", activeShaders, totalShaders);
    spdlog::info("Total:    {} active / {} cached", 
                 activeMeshes + activeTextures + activeShaders,
                 totalMeshes + totalTextures + totalShaders);
    spdlog::info("================================");
}


std::string AssetRegistry::normalizePath(const std::string& path)
{
    std::string normalized = path;
    
    // Trim whitespace
    auto start = normalized.find_first_not_of(" \t\n\r");
    auto end   = normalized.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos)
        return ""; // Empty or all whitespace
    
    normalized = normalized.substr(start, end - start + 1);
    
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    std::transform(normalized.begin(), normalized.end(), 
                   normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return normalized;
}

template<typename T>
std::shared_ptr<T> AssetRegistry::getCached(
    std::unordered_map<std::string, std::weak_ptr<T>>& cache,
    std::mutex& mutex,
    const std::string& key) const
{
    // Note: Caller must hold the lock already
    auto it = cache.find(key);
    if (it != cache.end())
    {
        // Try to lock weak_ptr
        if (auto locked = it->second.lock())
        {
            spdlog::debug("AssetRegistry: Cache hit for '{}'", key);
            return locked; // Still alive, return it
        }
        else
        {
            // Expired, remove entry
            spdlog::debug("AssetRegistry: Removing expired entry '{}'", key);
            cache.erase(it);
        }
    }
    
    return nullptr; // Not found or expired
}

template<typename T>
void AssetRegistry::insertCache(
    std::unordered_map<std::string, std::weak_ptr<T>>& cache,
    const std::string& key,
    std::shared_ptr<T> asset)
{
    cache[key] = asset;
    spdlog::debug("AssetRegistry: Cached asset '{}'", key);
}

template<typename T>
void AssetRegistry::cleanupCache(std::unordered_map<std::string, std::weak_ptr<T>>& cache)
{
    auto it = cache.begin();
    size_t removed = 0;
    
    while (it != cache.end())
    {
        if (it->second.expired())
        {
            it = cache.erase(it);
            ++removed;
        }
        else
        {
            ++it;
        }
    }
    
    if (removed > 0)
    {
        spdlog::debug("AssetRegistry: Removed {} expired entries from cache", removed);
    }
}

template<typename T>
size_t AssetRegistry::countActive(const std::unordered_map<std::string, std::weak_ptr<T>>& cache) const
{
    size_t active = 0;
    for (const auto& [key, weakPtr] : cache)
    {
        if (!weakPtr.expired())
            ++active;
    }
    return active;
}

// Explicit template instantiations
template std::shared_ptr<Mesh> AssetRegistry::getCached(
    std::unordered_map<std::string, std::weak_ptr<Mesh>>&, std::mutex&, const std::string&) const;
template std::shared_ptr<Texture> AssetRegistry::getCached(
    std::unordered_map<std::string, std::weak_ptr<Texture>>&, std::mutex&, const std::string&) const;
template std::shared_ptr<Shader> AssetRegistry::getCached(
    std::unordered_map<std::string, std::weak_ptr<Shader>>&, std::mutex&, const std::string&) const;

template void AssetRegistry::insertCache(
    std::unordered_map<std::string, std::weak_ptr<Mesh>>&, const std::string&, std::shared_ptr<Mesh>);
template void AssetRegistry::insertCache(
    std::unordered_map<std::string, std::weak_ptr<Texture>>&, const std::string&, std::shared_ptr<Texture>);
template void AssetRegistry::insertCache(
    std::unordered_map<std::string, std::weak_ptr<Shader>>&, const std::string&, std::shared_ptr<Shader>);

template void AssetRegistry::cleanupCache(std::unordered_map<std::string, std::weak_ptr<Mesh>>&);
template void AssetRegistry::cleanupCache(std::unordered_map<std::string, std::weak_ptr<Texture>>&);
template void AssetRegistry::cleanupCache(std::unordered_map<std::string, std::weak_ptr<Shader>>&);

template size_t AssetRegistry::countActive(const std::unordered_map<std::string, std::weak_ptr<Mesh>>&) const;
template size_t AssetRegistry::countActive(const std::unordered_map<std::string, std::weak_ptr<Texture>>&) const;
template size_t AssetRegistry::countActive(const std::unordered_map<std::string, std::weak_ptr<Shader>>&) const;
