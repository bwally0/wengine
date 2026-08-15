#pragma once

#include "wengine/render/VertexArray.h"
#include <string>
#include <vector>
#include <cstdint>

/**
 * @struct MeshData
 * @brief Represents raw mesh data.
 * 
 * Used as an intermediate representation before uploading to the GPU.
 */
struct MeshData
{
    std::vector<float>    vertices;   // [px, py, pz, nx, ny, nz, u, v, tx, ty, tz ...]
    std::vector<uint32_t> indices;
    std::vector<VertexAttribute> attributes;
    
    bool hasPositions = false;
    bool hasNormals   = false;
    bool hasTexCoords = false;
    bool hasTangents  = false;
    
    MeshData() = default;
    
    // Movable and disable copy
    MeshData(MeshData&& other) noexcept = default;
    MeshData& operator=(MeshData&& other) noexcept = default;
    MeshData(const MeshData&) = delete;
    MeshData& operator=(const MeshData&) = delete;
    
    bool isValid() const { return !vertices.empty() && !indices.empty(); }
    
    uint32_t getVertexCount() const;
    uint32_t getIndexCount() const { return static_cast<uint32_t>(indices.size()); }
    uint32_t getTriangleCount() const { return getIndexCount() / 3; }
};

/**
 * @class MeshLoader
 * @brief Utility class for loading mesh data from various file formats.
 * 
 * Uses the Assimp library to load meshes and convert them into a MeshData structure.
 */
class MeshLoader
{
public:
    /**
     * @brief Loads mesh data from a file.
     * 
     * @param path The file path to load the mesh from.
     * @return MeshData The loaded mesh data.
     */
    static MeshData load(const std::string& path);

    /**
     * @brief Loads mesh data from a file with specific flags.
     * 
     * @param path The file path to load the mesh from.
     * @param flags The Assimp processing flags to use when loading the mesh.
     * @return MeshData The loaded mesh data.
     */
    static MeshData load(const std::string& path, uint32_t flags);
    
    /**
     * @brief Loads a specific mesh from a file by index.
     * 
     * @param path The file path to load the mesh from.
     * @param meshIndex The index of the mesh to load.
     * @return MeshData The loaded mesh data.
     */
    static MeshData loadMeshAtIndex(const std::string& path, uint32_t meshIndex);
    
    /**
     * @brief Gets the number of meshes in a file.
     * 
     * @param path The file path to query.
     * @return uint32_t The number of meshes in the file.
     */
    static uint32_t getMeshCount(const std::string& path);

    /**
     * @brief Checks if a file format is supported by the mesh loader.
     * 
     * @param path The file path to check.
     * @return bool True if the format is supported, false otherwise.
     */
    static bool isSupported(const std::string& path);
    /**
     * @brief Gets the default Assimp processing flags for loading meshes.
     * 
     * @return uint32_t The default processing flags.
     */
    static uint32_t getDefaultFlags();

private:
    MeshLoader() = delete; // Static utility class
};
