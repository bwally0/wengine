#include "wengine/asset/loaders/MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstdint>

uint32_t MeshData::getVertexCount() const
{
    if (attributes.empty() || attributes[0].stride == 0)
        return 0;
    
    // Total bytes / bytes per vertex = vertex count
    size_t totalBytes = vertices.size() * sizeof(float);
    return static_cast<uint32_t>(totalBytes / attributes[0].stride);
}

uint32_t MeshLoader::getDefaultFlags()
{
    return aiProcess_Triangulate           |  // Convert all primitives to triangles
           aiProcess_GenNormals            |  // Generate normals if missing
           aiProcess_CalcTangentSpace      |  // Calculate tangents and bitangents
           aiProcess_JoinIdenticalVertices |  // Optimize by merging identical vertices
           aiProcess_SortByPType           |  // Split meshes by primitive type
           aiProcess_FlipUVs;                 // Flip UVs (OpenGL texture coords)
}

MeshData MeshLoader::load(const std::string& path)
{
    return load(path, getDefaultFlags());
}

MeshData MeshLoader::load(const std::string& path, uint32_t flags)
{
    Assimp::Importer importer;
    
    // Load the scene
    const aiScene* scene = importer.ReadFile(path, flags);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string error = importer.GetErrorString();
        spdlog::error("MeshLoader: failed to load '{}': {}", path, error);
        throw std::runtime_error("MeshLoader: failed to load '" + path + "': " + error);
    }
    
    if (scene->mNumMeshes == 0)
    {
        spdlog::error("MeshLoader: no meshes found in '{}'", path);
        throw std::runtime_error("MeshLoader: no meshes found in '" + path + "'");
    }
    
    // Load the first mesh
    MeshData data = loadMeshAtIndex(path, 0);
    
    spdlog::info("MeshLoader: loaded '{}' ({} vertices, {} triangles)", 
                 path, data.getVertexCount(), data.getTriangleCount());
    
    return data;
}

MeshData MeshLoader::loadMeshAtIndex(const std::string& path, uint32_t meshIndex)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, getDefaultFlags());
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("MeshLoader: failed to load '" + path + "'");
    }
    
    if (meshIndex >= scene->mNumMeshes)
    {
        throw std::runtime_error("MeshLoader: mesh index out of range");
    }
    
    const aiMesh* mesh = scene->mMeshes[meshIndex];
    
    MeshData data;
    data.hasPositions = mesh->HasPositions();
    data.hasNormals   = mesh->HasNormals();
    data.hasTexCoords = mesh->HasTextureCoords(0);
    data.hasTangents  = mesh->HasTangentsAndBitangents();
    
    // Calculate vertex stride and setup attributes
    uint32_t stride = 0;
    uint32_t currentOffset = 0;
    uint32_t attributeIndex = 0;
    
    // Position (always required)
    if (data.hasPositions)
    {
        stride += 3 * sizeof(float);
    }
    
    // Normal
    if (data.hasNormals)
    {
        stride += 3 * sizeof(float);
    }
    
    // TexCoord
    if (data.hasTexCoords)
    {
        stride += 2 * sizeof(float);
    }
    
    // Tangent
    if (data.hasTangents)
    {
        stride += 3 * sizeof(float);
    }
    
    // Build attribute descriptors
    if (data.hasPositions)
    {
        data.attributes.push_back({
            attributeIndex++,      // index
            3,                     // size (vec3)
            GL_FLOAT,              // type
            false,                 // normalized
            stride,                // stride
            currentOffset          // offset
        });
        currentOffset += 3 * sizeof(float);
    }
    
    if (data.hasNormals)
    {
        data.attributes.push_back({
            attributeIndex++,
            3,
            GL_FLOAT,
            false,
            stride,
            currentOffset
        });
        currentOffset += 3 * sizeof(float);
    }
    
    if (data.hasTexCoords)
    {
        data.attributes.push_back({
            attributeIndex++,
            2,
            GL_FLOAT,
            false,
            stride,
            currentOffset
        });
        currentOffset += 2 * sizeof(float);
    }
    
    if (data.hasTangents)
    {
        data.attributes.push_back({
            attributeIndex++,
            3,
            GL_FLOAT,
            false,
            stride,
            currentOffset
        });
        currentOffset += 3 * sizeof(float);
    }
    
    // Reserve space for interleaved vertex data
    uint32_t floatsPerVertex = stride / sizeof(float);
    data.vertices.reserve(mesh->mNumVertices * floatsPerVertex);
    
    // Interleave vertex data
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        if (data.hasPositions)
        {
            data.vertices.push_back(mesh->mVertices[i].x);
            data.vertices.push_back(mesh->mVertices[i].y);
            data.vertices.push_back(mesh->mVertices[i].z);
        }
        
        if (data.hasNormals)
        {
            data.vertices.push_back(mesh->mNormals[i].x);
            data.vertices.push_back(mesh->mNormals[i].y);
            data.vertices.push_back(mesh->mNormals[i].z);
        }
        
        if (data.hasTexCoords)
        {
            data.vertices.push_back(mesh->mTextureCoords[0][i].x);
            data.vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        
        if (data.hasTangents)
        {
            data.vertices.push_back(mesh->mTangents[i].x);
            data.vertices.push_back(mesh->mTangents[i].y);
            data.vertices.push_back(mesh->mTangents[i].z);
        }
    }
    
    // Extract indices
    data.indices.reserve(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            data.indices.push_back(face.mIndices[j]);
        }
    }
    
    return data;
}

uint32_t MeshLoader::getMeshCount(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        return 0;
    }
    
    return scene->mNumMeshes;
}

bool MeshLoader::isSupported(const std::string& path)
{
    std::string ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == "obj" || ext == "fbx" || ext == "stl";
}
