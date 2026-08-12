#include "wengine/render/Mesh.h"
#include "wengine/asset/loaders/MeshLoader.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

std::shared_ptr<Mesh> Mesh::create(const std::string& path)
{
    MeshData data = MeshLoader::load(path);
    return create(data);
}

std::shared_ptr<Mesh> Mesh::create(const MeshData& data)
{
    if (!data.isValid())
    {
        throw std::runtime_error("Mesh::create: invalid MeshData (empty vertices or indices)");
    }
    
    if (data.attributes.empty())
    {
        throw std::runtime_error("Mesh::create: MeshData has no vertex attributes");
    }
    
    auto mesh = std::shared_ptr<Mesh>(new Mesh());
    
    // Create vertex buffer with interleaved data
    mesh->m_vbo = std::make_shared<VertexBuffer>(
        data.vertices.data(),
        data.vertices.size() * sizeof(float)
    );
    
    // Create index buffer
    mesh->m_ibo = std::make_shared<IndexBuffer>(
        data.indices.data(),
        static_cast<uint32_t>(data.indices.size())
    );
    
    // Create vertex array and configure attributes
    mesh->m_vao = std::make_shared<VertexArray>();
    mesh->m_vao->addVertexBuffer(*mesh->m_vbo, data.attributes);
    mesh->m_vao->setIndexBuffer(*mesh->m_ibo);
    
    // Store counts
    mesh->m_vertexCount = data.getVertexCount();
    mesh->m_indexCount = data.getIndexCount();
    
    spdlog::info("Mesh: created ({} vertices, {} triangles)", 
                 mesh->m_vertexCount, mesh->getTriangleCount());
    
    return mesh;
}
