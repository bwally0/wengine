#pragma once

#include "wengine/render/VertexArray.h"
#include <string>
#include <vector>
#include <cstdint>

// Intermediate mesh data structure
// Holds raw geometry data loaded from disk before GPU upload
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
    
    // Movable
    MeshData(MeshData&& other) noexcept = default;
    MeshData& operator=(MeshData&& other) noexcept = default;
    
    // Disable copy
    MeshData(const MeshData&) = delete;
    MeshData& operator=(const MeshData&) = delete;
    
    bool isValid() const { return !vertices.empty() && !indices.empty(); }
    
    uint32_t getVertexCount() const;
    uint32_t getIndexCount() const { return static_cast<uint32_t>(indices.size()); }
    uint32_t getTriangleCount() const { return getIndexCount() / 3; }
};

class MeshLoader
{
public:
    static MeshData load(const std::string& path);
    static MeshData load(const std::string& path, uint32_t flags);
    
    static MeshData loadMeshAtIndex(const std::string& path, uint32_t meshIndex);
    
    static uint32_t getMeshCount(const std::string& path);

    static bool isSupported(const std::string& path);
    static uint32_t getDefaultFlags();

private:
    MeshLoader() = delete; // Static utility class
};
