#pragma once

#include "wengine/render/VertexBuffer.h"
#include "wengine/render/IndexBuffer.h"
#include <cstdint>
#include <vector>

struct VertexAttribute
{
    uint32_t index;      // layout location in shader (layout(location = N))
    int      size;       // number of components (e.g. 3 for vec3)
    uint32_t type;       // GL_FLOAT, GL_UNSIGNED_INT, etc.
    bool     normalized; // normalize fixed-point values
    uint32_t stride;     // byte offset between consecutive vertices
    size_t   offset;     // byte offset of this attribute within a vertex
};

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&)            = delete; // non-copyable
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;           // movable
    VertexArray& operator=(VertexArray&& other) noexcept;

    void bind()   const;
    void unbind() const;

    void addVertexBuffer(const VertexBuffer& vbo, const std::vector<VertexAttribute>& attributes);
    void setIndexBuffer(const IndexBuffer& ibo);

private:
    uint32_t m_id = 0;
};
