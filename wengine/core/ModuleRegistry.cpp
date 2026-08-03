#include "wengine/core/ModuleRegistry.h"

#include <spdlog/spdlog.h>
#include <functional>
#include <stdexcept>

void ModuleRegistry::registerModule(std::string name, std::unique_ptr<IModule> module)
{
    m_modules.push_back({ std::move(name), std::move(module) });
    m_sortedValid = false;
}

void ModuleRegistry::init()
{
    sortModules();

    for (auto* module : m_sortedModules)
        module->init();
}

void ModuleRegistry::update(double deltaTime)
{
    if (!m_sortedValid) return;

    for (auto* module : m_sortedModules)
        module->update(deltaTime);
}

void ModuleRegistry::render()
{
    if (!m_sortedValid) return;

    for (auto* module : m_sortedModules)
        module->render();
}

void ModuleRegistry::shutdown()
{
    if (!m_sortedValid) return;

    for (auto it = m_sortedModules.rbegin(); it != m_sortedModules.rend(); ++it)
        (*it)->shutdown();
}

void ModuleRegistry::sortModules()
{
    m_sortedModules.clear();

    enum class State { NOT_VISITED, IN_PROGRESS, VISITED };

    std::unordered_map<std::string, State>   visited;   // module name -> visit state
    std::unordered_map<std::string, IModule*> moduleMap; // module name -> module object

    for (auto& entry : m_modules)
    {
        moduleMap[entry.name] = entry.module.get();
        visited[entry.name]   = State::NOT_VISITED;
    }

    std::function<void(const std::string&)> visit = [&](const std::string& name)
    {
        if (visited[name] == State::VISITED) return;
        if (visited[name] == State::IN_PROGRESS)
        {
            spdlog::error("ModuleRegistry: circular dependency found for '{}'", name);
            throw std::runtime_error("ModuleRegistry: circular dependency found");
        }

        visited[name] = State::IN_PROGRESS;

        for (const auto& dependency : moduleMap[name]->getDependencies())
        {
            if (moduleMap.find(dependency) != moduleMap.end())
            {
                visit(dependency);
            }
            else
            {
                spdlog::error("ModuleRegistry: module '{}' requires missing dependency '{}'", name, dependency);
                throw std::runtime_error("ModuleRegistry: dependency not found");
            }
        }

        visited[name] = State::VISITED;
        m_sortedModules.push_back(moduleMap[name]);
    };

    for (auto& entry : m_modules) visit(entry.name);

    m_sortedValid = true;
}
