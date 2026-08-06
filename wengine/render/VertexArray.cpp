#include "wengine/render/VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    if (m_id) glDeleteVertexArrays(1, &m_id);
}

// move object
VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

// move object
VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        if (m_id) glDeleteVertexArrays(1, &m_id);
        m_id       = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

void VertexArray::bind()   const { glBindVertexArray(m_id); }
void VertexArray::unbind() const { glBindVertexArray(0); }

void VertexArray::addVertexBuffer(const VertexBuffer& vbo, const std::vector<VertexAttribute>& attributes)
{
    glBindVertexArray(m_id);
    vbo.bind();

    for (const auto& attr : attributes)
    {
        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(
            attr.index,
            attr.size,
            attr.type,
            attr.normalized ? GL_TRUE : GL_FALSE,
            attr.stride,
            reinterpret_cast<const void*>(attr.offset)
        );
    }

    glBindVertexArray(0);
}
