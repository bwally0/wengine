#pragma once

#include "wengine/render/VertexArray.h"
#include "wengine/render/VertexBuffer.h"
#include "wengine/render/IndexBuffer.h"

#include <string>
#include <memory>
#include <cstdint>

struct MeshData;

class Mesh
{
public:
    static std::shared_ptr<Mesh> create(const std::string& path);
    static std::shared_ptr<Mesh> create(const MeshData& data);
    
    ~Mesh() = default;

    Mesh(const Mesh&)            = delete;  // non-copyable
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept = default;  // movable
    Mesh& operator=(Mesh&& other) noexcept = default;

    void bind() const { m_vao->bind(); }
    void unbind() const { m_vao->unbind(); }
    
    uint32_t getIndexCount() const { return m_indexCount; }
    uint32_t getVertexCount() const { return m_vertexCount; }
    uint32_t getTriangleCount() const { return m_indexCount / 3; }
    
    std::shared_ptr<VertexArray> getVertexArray() const { return m_vao; }
    std::shared_ptr<IndexBuffer> getIndexBuffer() const { return m_ibo; }

private:
    Mesh() = default; // Private for factory pattern
    
    std::shared_ptr<VertexArray>  m_vao;
    std::shared_ptr<VertexBuffer> m_vbo;
    std::shared_ptr<IndexBuffer>  m_ibo;
    
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount  = 0;
};
