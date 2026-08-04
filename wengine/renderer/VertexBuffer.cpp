#include "wengine/renderer/VertexBuffer.h"

#include <glad/glad.h>

VertexBuffer::VertexBuffer(const void* data, size_t size)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    if (m_id) glDeleteBuffers(1, &m_id);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_id) glDeleteBuffers(1, &m_id);
        m_id       = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

void VertexBuffer::bind()   const { glBindBuffer(GL_ARRAY_BUFFER, m_id); }
void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
