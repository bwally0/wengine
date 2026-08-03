#pragma once

#include "wengine/scene/EntityManager.h"

#include <spdlog/spdlog.h>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

struct IComponentPool
{
    virtual void remove(EntityID id) = 0;
    virtual ~IComponentPool() = default;
};

// typed sparse set pool for one component type
template<typename TComponent>
struct ComponentPool : public IComponentPool
{
    void add(EntityID id, TComponent component)
    {
        if (m_sparse.count(id))
        {
            spdlog::warn("ComponentPool: entity {} already has this component, overwriting", id);
            m_denseComponents[m_sparse[id]] = std::move(component);
            return;
        }

        m_sparse[id] = m_denseComponents.size();
        m_denseComponents.push_back(std::move(component));
        m_denseEntities.push_back(id);
    }

    TComponent* get(EntityID id)
    {
        auto it = m_sparse.find(id);
        if (it == m_sparse.end()) return nullptr;
        return &m_denseComponents[it->second];
    }

    void remove(EntityID id) override
    {
        auto it = m_sparse.find(id);
        if (it == m_sparse.end()) return;

        size_t index     = it->second;
        size_t lastIndex = m_denseComponents.size() - 1;

        if (index != lastIndex)
        {
            // swap with last to keep dense array packed
            m_denseComponents[index] = std::move(m_denseComponents[lastIndex]);
            EntityID lastEntity      = m_denseEntities[lastIndex];
            m_denseEntities[index]   = lastEntity;
            m_sparse[lastEntity]     = index;
        }

        m_denseComponents.pop_back();
        m_denseEntities.pop_back();
        m_sparse.erase(id);
    }

    bool has(EntityID id) const
    {
        return m_sparse.count(id) > 0;
    }

    std::vector<TComponent>& components()            { return m_denseComponents; }
    std::vector<EntityID>&   entities()              { return m_denseEntities; }

private:
    std::vector<TComponent>              m_denseComponents; // packed components
    std::vector<EntityID>                m_denseEntities;   // entity for each dense slot
    std::unordered_map<EntityID, size_t> m_sparse;          // entity -> dense index
};


class ComponentStorage
{
public:
    template<typename TComponent>
    void add(EntityID id, TComponent component)
    {
        getPool<TComponent>().add(id, std::move(component));
    }

    template<typename TComponent>
    TComponent* get(EntityID id)
    {
        auto it = m_pools.find(std::type_index(typeid(TComponent)));
        if (it == m_pools.end()) return nullptr;
        return static_cast<ComponentPool<TComponent>*>(it->second.get())->get(id);
    }

    template<typename TComponent>
    void remove(EntityID id)
    {
        auto it = m_pools.find(std::type_index(typeid(TComponent)));
        if (it != m_pools.end())
            it->second->remove(id);
    }

    template<typename TComponent>
    bool has(EntityID id)
    {
        auto it = m_pools.find(std::type_index(typeid(TComponent)));
        if (it == m_pools.end()) return false;
        return static_cast<ComponentPool<TComponent>*>(it->second.get())->has(id);
    }

    // remove all components for an entity
    void removeAll(EntityID id)
    {
        for (auto& [key, pool] : m_pools)
            pool->remove(id);
    }

    template<typename TComponent>
    ComponentPool<TComponent>& getPool()
    {
        auto key = std::type_index(typeid(TComponent));
        auto it  = m_pools.find(key);

        if (it == m_pools.end())
        {
            m_pools[key] = std::make_unique<ComponentPool<TComponent>>();
            return *static_cast<ComponentPool<TComponent>*>(m_pools[key].get());
        }

        return *static_cast<ComponentPool<TComponent>*>(it->second.get());
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_pools;
};
