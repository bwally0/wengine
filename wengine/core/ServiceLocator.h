#pragma once

#include <typeindex>
#include <unordered_map>
#include <spdlog/spdlog.h>

class ServiceLocator
{
public:
    template<typename TService>
    void provide(TService* service)
    {
        auto key = std::type_index(typeid(TService));
        if (m_services.count(key))
            spdlog::warn("overwriting existing service: {}", typeid(TService).name());

        m_services[key] = service;
    }

    template<typename TService>
    TService* get()
    {
        auto it = m_services.find(std::type_index(typeid(TService)));
        if (it == m_services.end())
        {
            spdlog::error("service {} is not provided", typeid(TService).name());
            return nullptr;
        }
        return static_cast<TService*>(it->second);
    }

    template<typename TService>
    bool has() const
    {
        return m_services.count(std::type_index(typeid(TService))) > 0;
    }

private:
    std::unordered_map<std::type_index, void*> m_services;
};