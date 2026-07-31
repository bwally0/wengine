#pragma once

#include "IModule.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class ModuleRegistry
{
public:
    void registerModule(std::string name, std::unique_ptr<IModule> module);

    void init();
    void update(double deltaTime);
    void render();
    void shutdown();

private:
    struct ModuleEntry
    {
        std::string name;
        std::unique_ptr<IModule> module;
    };

    void sortModules(); // populates m_sorted_modules, topological sort

    std::vector<ModuleEntry> m_modules;
    std::vector<IModule*> m_sorted_modules;
    bool m_sorted_valid = false;
};