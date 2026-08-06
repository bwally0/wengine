#pragma once

#include "wengine/render/VertexArray.h"
#include "wengine/render/IndexBuffer.h"
#include "wengine/render/Shader.h"
#include <memory>
#include <cstdint>

struct Mesh
{
    std::shared_ptr<VertexArray> vertexArray;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<Shader>      shader;
};
