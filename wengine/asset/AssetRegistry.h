#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

class Mesh;
class Texture;
class Shader;
class Material;

/**
 * @class AssetRegistry
 * @brief Manages the loading and caching of assets such as meshes, textures, and shaders.
 */
class AssetRegistry
{
public:
    AssetRegistry() = default;
    ~AssetRegistry() = default;

    // Singleton
    AssetRegistry(const AssetRegistry&) = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;
    AssetRegistry(AssetRegistry&&) = delete;
    AssetRegistry& operator=(AssetRegistry&&) = delete;

    /** @brief Loads a mesh from the specified path. 
     *  
     * @param path The file path to the mesh asset.
     * @return std::shared_ptr<Mesh> A shared pointer to the loaded mesh, or nullptr if loading failed.
     */
    std::shared_ptr<Mesh> loadMesh(const std::string& path);

    /** @brief Loads a texture from the specified path.
     *  
     * @param path The file path to the texture asset.
     * @return std::shared_ptr<Texture> A shared pointer to the loaded texture, or nullptr if loading failed.
     */
    std::shared_ptr<Texture> loadTexture(const std::string& path);

    /** @brief Loads a shader from the specified vertex and fragment shader paths.
     *  
     * @param vertexPath The file path to the vertex shader.
     * @param fragmentPath The file path to the fragment shader.
     * @return std::shared_ptr<Shader> A shared pointer to the loaded shader, or nullptr if loading failed.
     */
    std::shared_ptr<Shader> loadShader(
        const std::string& vertexPath,
        const std::string& fragmentPath
    );

    /** @brief Creates a material using the specified shader and optional texture maps.
     *  
     * @param shaderVert The file path to the vertex shader.
     * @param shaderFrag The file path to the fragment shader.
     * @param diffusePath Optional file path to the diffuse/albedo texture.
     * @param normalPath Optional file path to the normal map texture.
     * @param specularPath Optional file path to the specular/metallic-roughness texture.
     * @param emissivePath Optional file path to the emissive texture.
     * @return std::shared_ptr<Material> A shared pointer to the created material, or nullptr if creation failed.
     */
    std::shared_ptr<Material> createMaterial(
        const std::string& shaderVert,
        const std::string& shaderFrag,
        const std::string& diffusePath = "",
        const std::string& normalPath = "",
        const std::string& specularPath = "",
        const std::string& emissivePath = ""
    );

    /** @brief Checks if a mesh is already cached for the specified path.
     *  
     * @param path The file path to the mesh asset.
     * @return true If the mesh is cached and valid.
     * @return false If the mesh is not cached or has expired.
     */
    bool hasMesh(const std::string& path) const;

    /** @brief Checks if a texture is already cached for the specified path.
     *  
     * @param path The file path to the texture asset.
     * @return true If the texture is cached and valid.
     * @return false If the texture is not cached or has expired.
     */
    bool hasTexture(const std::string& path) const;

    /** @brief Checks if a shader is already cached for the specified paths.
     *  
     * @param vertexPath The file path to the vertex shader.
     * @param fragmentPath The file path to the fragment shader.
     * @return true If the shader is cached and valid.
     * @return false If the shader is not cached or has expired.
     */
    bool hasShader(const std::string& vertexPath, const std::string& fragmentPath) const;

    /** @brief Gets the number of cached meshes.
     *  
     * @return size_t The number of cached meshes.
     */
    size_t getMeshCacheSize() const;

    /** @brief Gets the number of cached textures.
     *  
     * @return size_t The number of cached textures.
     */
    size_t getTextureCacheSize() const;

    /** @brief Gets the number of cached shaders.
     *  
     * @return size_t The number of cached shaders.
     */
    size_t getShaderCacheSize() const;

    /** @brief Gets the total number of active assets.
     *  
     * @return size_t The number of active assets.
     */
    size_t getActiveAssetCount() const;

    /** @brief Cleans up expired assets from the cache.
     *  
     * Removes any assets that are no longer referenced by any shared pointers.
     */
    void cleanupExpired();

    /** @brief Clears all cached assets.
     *  
     * Removes all assets from the cache, regardless of whether they are still in use.
     */
    void clear();

    /** @brief Logs statistics about the asset registry.
     *  
     * Outputs the number of active and cached assets for meshes, textures, and shaders.
     */
    void logStatistics() const;

private:
    /** @brief Normalizes a file path for consistent caching.
     * 
     * @param path The file path to normalize.
     * @return std::string The normalized file path.
     */
    static std::string normalizePath(const std::string& path);

    /** @brief Cleans up expired entries in a specific cache.
     * 
     * @tparam TAsset The type of asset (Mesh, Texture, Shader).
     * @param cache The cache to clean up.
     */
    template<typename TAsset>
    std::shared_ptr<TAsset> getCached(
        std::unordered_map<std::string, std::weak_ptr<TAsset>>& cache,
        std::mutex& mutex,
        const std::string& key
    ) const;

    /** @brief Inserts an asset into the cache.
     * 
     * @tparam TAsset The type of asset (Mesh, Texture, Shader).
     * @param cache The cache to insert into.
     * @param key The key for the asset in the cache.
     * @param asset The shared pointer to the asset to cache.
     */
    template<typename TAsset>
    void insertCache(
        std::unordered_map<std::string, std::weak_ptr<TAsset>>& cache,
        const std::string& key,
        std::shared_ptr<TAsset> asset
    );

    /** @brief Cleans up expired entries in a specific cache.
     * 
     * @tparam TAsset The type of asset (Mesh, Texture, Shader).
     * @param cache The cache to clean up.
     */
    template<typename TAsset>
    void cleanupCache(std::unordered_map<std::string, std::weak_ptr<TAsset>>& cache);

    /** @brief Counts the number of active (non-expired) assets in a specific cache.
     * 
     * @tparam TAsset The type of asset (Mesh, Texture, Shader).
     * @param cache The cache to count active assets in.
     * @return size_t The number of active assets.
     */
    template<typename TAsset>
    size_t countActive(const std::unordered_map<std::string, std::weak_ptr<TAsset>>& cache) const;

    std::unordered_map<std::string, std::weak_ptr<Mesh>>    m_meshCache;      ///< Cache for loaded meshes
    std::unordered_map<std::string, std::weak_ptr<Texture>> m_textureCache;   ///< Cache for loaded textures
    std::unordered_map<std::string, std::weak_ptr<Shader>>  m_shaderCache;    ///< Cache for loaded shaders

    mutable std::mutex m_meshMutex;
    mutable std::mutex m_textureMutex;
    mutable std::mutex m_shaderMutex;
    
    static constexpr size_t CLEANUP_THRESHOLD = 100;
};
