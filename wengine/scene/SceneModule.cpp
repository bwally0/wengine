#include "wengine/scene/SceneModule.h"

#include <spdlog/spdlog.h>

void SceneModule::init()
{
    spdlog::info("SceneModule: initialized");
}

void SceneModule::update(double deltaTime)
{
    m_systemRegistry.update(deltaTime);
}

void SceneModule::shutdown()
{
    spdlog::info("SceneModule: shutdown");
}

EntityID SceneModule::createEntity()
{
   return m_entityManager.create(); 
}

void SceneModule::destroyEntity(EntityID id)
{
    m_componentStorage.removeAll(id);
    m_entityManager.destroy(id);
}

bool SceneModule::isAlive(EntityID id) const
{
    return m_entityManager.isAlive(id);
}

void SceneModule::registerSystem(std::string name, std::unique_ptr<ISystem> system)
{
    m_systemRegistry.registerSystem(std::move(name), std::move(system));
}