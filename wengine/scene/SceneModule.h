#pragma once

#include "wengine/core/IModule.h"
#include "wengine/scene/EntityManager.h"
#include "wengine/scene/ComponentStorage.h"
#include "wengine/scene/SystemRegistry.h"

class SceneModule : public IModule
{
public:
    void init()                   override;
    void update(double deltaTime) override;
    void render()                 override {}
    void shutdown()               override;

    EntityID createEntity();
    void destroyEntity(EntityID id);
    bool isAlive(EntityID id) const;

    template<typename TComponent>
    void addComponent(EntityID id, TComponent component)
    {
        m_componentStorage.add<TComponent>(id, std::move(component));
    }

    template<typename TComponent>
    TComponent* getComponent(EntityID id) const
    {
        return m_componentStorage.get<TComponent>(id);
    }

    template<typename TComponent>
    bool hasComponent(EntityID id) const
    {
        return m_componentStorage.has<TComponent>(id);
    }

    template<typename TComponent>
    void removeComponent(EntityID id)
    {
        m_componentStorage.remove<TComponent>(id);
    }

    void registerSystem(std::string name, std::unique_ptr<ISystem> system);

    template<typename TComponent>
    ComponentPool<TComponent>& query()
    {
        return m_componentStorage.getPool<TComponent>();
    }

private:
    EntityManager    m_entityManager;
    ComponentStorage m_componentStorage;
    SystemRegistry   m_systemRegistry;
};