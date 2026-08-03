#include "wengine/scene/SystemRegistry.h"

#include <spdlog/spdlog.h>

void SystemRegistry::registerSystem(std::string name, std::unique_ptr<ISystem> system)
{
    spdlog::info("SystemRegistry: registering system '{}'", name);
    m_systems.push_back({ std::move(name), std::move(system)});
}

void SystemRegistry::update(double deltaTime)
{
    for (auto& entry : m_systems)
        entry.system->update(deltaTime);
}