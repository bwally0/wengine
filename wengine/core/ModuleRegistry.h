#pragma once

#include "wengine/core/IModule.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class ResourceRegistry;

class ModuleRegistry
{
public:
    void registerModule(std::string name, std::unique_ptr<IModule> module);

    void init(ResourceRegistry& resources);
    void update(double deltaTime);
    void render();
    void shutdown();
    void clear();

private:
    struct ModuleEntry
    {
        std::string              name;
        std::unique_ptr<IModule> module;
    };

    void sortModules();

    std::vector<ModuleEntry> m_modules;
    std::vector<IModule*>    m_sortedModules;
    bool                     m_sortedValid = false;
};
