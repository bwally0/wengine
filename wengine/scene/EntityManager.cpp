#include "wengine/scene/EntityManager.h"

#include <spdlog/spdlog.h>

EntityID EntityManager::create()
{
    EntityID id;

    if (m_freeList.empty())
    {
        id = m_nextID++;
    } 
    else 
    {
        // reuse free id
        id = m_freeList.back();
        m_freeList.pop_back();
    }

    m_aliveEntities.insert(id);
    return id;
}


void EntityManager::destroy(EntityID id)
{
    if (!isAlive(id)) {
        spdlog::warn("EntityManager: tried to destroy an entity that does not exist '{}'", id);
        return;
    }

    m_aliveEntities.erase(id);
    m_freeList.push_back(id);
}

bool EntityManager::isAlive(EntityID id) const
{
    return m_aliveEntities.count(id) > 0;
}