#pragma once

#include <cstdint>
#include <cstddef>

class VertexBuffer
{
public:
    VertexBuffer(const void* data, size_t size);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&)            = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void bind()   const;
    void unbind() const;

private:
    uint32_t m_id = 0;
};
