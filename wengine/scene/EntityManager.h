#pragma once

#include <cstdint>
#include <vector>
#include <unordered_set>

using EntityID = uint32_t;
const EntityID NULL_ENTITY = 0;

class EntityManager
{
public:
    EntityID create();
    void destroy(EntityID id);
    bool isAlive(EntityID id);

private:
    EntityID                     m_nextID = 1; // 0 is NULL_ENTITY
    std::vector<bool>            m_freeList;
    std::unordered_set<EntityID> m_aliveEntities;
};