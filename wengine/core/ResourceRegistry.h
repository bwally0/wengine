#pragma once

#include <typeindex>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <stdexcept>

class ResourceRegistry
{
public:
    template<typename TResource>
    void provide(TResource* resource)
    {
        auto key = std::type_index(typeid(TResource));
        if (m_resources.count(key))
            spdlog::warn("ResourceRegistry: overwriting existing resource '{}'", typeid(TResource).name());

        m_resources[key] = resource;
    }

    template<typename TResource>
    TResource& get()
    {
        auto it = m_resources.find(std::type_index(typeid(TResource)));
        if (it == m_resources.end())
        {
            spdlog::error("ResourceRegistry: resource '{}' is not provided", typeid(TResource).name());
            throw std::runtime_error("ResourceRegistry: missing resource");
        }
        return *static_cast<TResource*>(it->second);
    }

    template<typename TResource>
    bool has() const
    {
        return m_resources.count(std::type_index(typeid(TResource))) > 0;
    }

private:
    std::unordered_map<std::type_index, void*> m_resources;
};
