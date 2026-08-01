#include "ModuleRegistry.h"
#include <spdlog/spdlog.h>
#include <functional>
#include <stdexcept>

void ModuleRegistry::registerModule(std::string name, std::unique_ptr<IModule> module)
{
    m_modules.push_back({std::move(name), std::move(module)}); // add module entry
    m_sorted_valid = false;
}

void ModuleRegistry::init()
{
    sortModules(); // populates m_sorted_modules, topological sort
    
    for (auto* module : m_sorted_modules)
    {
        module->init();
    }
}

void ModuleRegistry::update(double deltaTime)
{
    if (!m_sorted_valid) return;

    for (auto* module : m_sorted_modules)
    {
        module->update(deltaTime);
    }
}

void ModuleRegistry::render()
{
    if (!m_sorted_valid) return;

    for (auto* module : m_sorted_modules)
    {
        module->render();
    }
}

void ModuleRegistry::shutdown()
{
    if (!m_sorted_valid) return;

    for (auto it = m_sorted_modules.rbegin(); it != m_sorted_modules.rend(); ++it)
    {
        (*it)->shutdown();
    }
}

void ModuleRegistry::sortModules()
{
    m_sorted_modules.clear();

    // keep track of state to prevent circular dependencies
    enum class State
    {
        NOT_VISITED,
        IN_PROGRESS,
        VISITED
    };

    std::unordered_map<std::string, State> visited;       // module name -> visit state
    std::unordered_map<std::string, IModule*> module_map; // module name -> module object

    for (auto& entry : m_modules)
    {
        module_map[entry.name] = entry.module.get();
        visited[entry.name] = State::NOT_VISITED;
    }

    std::function<void(const std::string&)> visit = [&] (const std::string& name)
    {
        if (visited[name] == State::VISITED) return;
        if (visited[name] == State::IN_PROGRESS)
        {
            spdlog::error("circular dependency found for module: {}", name);
            throw std::runtime_error("circular dependency: " + name);
        }

        visited[name] = State::IN_PROGRESS;

        for (const auto& dependency : module_map[name]->getDependencies())
        {
            if (module_map.find(dependency) != module_map.end())
            {
                visit(dependency);
            }
            else
            {
                spdlog::error("module {} requires missing dependency: {}", name, dependency);
                throw std::runtime_error("dependency not found: " + dependency);
            }
        }

        visited[name] = State::VISITED;
        m_sorted_modules.push_back(module_map[name]);
    };

    for (auto& entry : m_modules) visit(entry.name);


    m_sorted_valid = true;
}