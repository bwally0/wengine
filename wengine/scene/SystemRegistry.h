#pragma once

#include "wengine/core/ISystem.h"
#include <memory>
#include <string>
#include <vector>

class SystemRegistry
{
public:
    void registerSystem(std::string name, std::unique_ptr<ISystem> system);
    void update(double deltaTime);

private:
    struct SystemEntry
    {
        std::string              name;
        std::unique_ptr<ISystem> system;
    };

    std::vector<SystemEntry> m_systems;
};