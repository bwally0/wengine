#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

class Mesh;
class Texture;
class Shader;
class Material;

class AssetRegistry
{
public:
    AssetRegistry() = default;
    ~AssetRegistry() = default;

    AssetRegistry(const AssetRegistry&) = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;
    AssetRegistry(AssetRegistry&&) = delete;
    AssetRegistry& operator=(AssetRegistry&&) = delete;

    std::shared_ptr<Mesh> loadMesh(const std::string& path);
    std::shared_ptr<Texture> loadTexture(const std::string& path);
    std::shared_ptr<Shader> loadShader(
        const std::string& vertexPath,
        const std::string& fragmentPath
    );

    std::shared_ptr<Material> createMaterial(
        const std::string& shaderVert,
        const std::string& shaderFrag,
        const std::string& diffusePath = ""
    );


    bool hasMesh(const std::string& path) const;
    bool hasTexture(const std::string& path) const;
    bool hasShader(const std::string& vertexPath, const std::string& fragmentPath) const;


    size_t getMeshCacheSize() const;
    size_t getTextureCacheSize() const;
    size_t getShaderCacheSize() const;

    size_t getActiveAssetCount() const;


    void cleanupExpired();
    void clear();

    void logStatistics() const;

private:
    static std::string normalizePath(const std::string& path);

    template<typename T>
    std::shared_ptr<T> getCached(
        std::unordered_map<std::string, std::weak_ptr<T>>& cache,
        std::mutex& mutex,
        const std::string& key
    ) const;

    template<typename T>
    void insertCache(
        std::unordered_map<std::string, std::weak_ptr<T>>& cache,
        const std::string& key,
        std::shared_ptr<T> asset
    );


    template<typename T>
    void cleanupCache(std::unordered_map<std::string, std::weak_ptr<T>>& cache);

    template<typename T>
    size_t countActive(const std::unordered_map<std::string, std::weak_ptr<T>>& cache) const;

    std::unordered_map<std::string, std::weak_ptr<Mesh>>    m_meshCache;
    std::unordered_map<std::string, std::weak_ptr<Texture>> m_textureCache;
    std::unordered_map<std::string, std::weak_ptr<Shader>>  m_shaderCache;

    mutable std::mutex m_meshMutex;
    mutable std::mutex m_textureMutex;
    mutable std::mutex m_shaderMutex;
    
    static constexpr size_t CLEANUP_THRESHOLD = 100;
};
