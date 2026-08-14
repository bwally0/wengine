#pragma once

#include <memory>

class Material;

struct RenderComponent
{
    std::shared_ptr<Material> material;
};
