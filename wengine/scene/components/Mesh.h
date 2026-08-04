#pragma once

#include "wengine/renderer/VertexArray.h"
#include "wengine/renderer/IndexBuffer.h"
#include "wengine/renderer/Shader.h"
#include <memory>
#include <cstdint>

struct Mesh
{
    std::shared_ptr<VertexArray> vertexArray;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<Shader>      shader;
};
