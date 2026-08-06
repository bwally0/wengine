#pragma once

#include <typeindex>
#include <vector>
#include <unordered_map>
#include <functional>

// TODO: switch to EventQueue if this becomes a problem
class EventBus
{
public:
    template<typename TEvent>
    void subscribe(std::function<void(const TEvent&)> callback)
    {
        auto key = std::type_index(typeid(TEvent));

        m_subscriberCallbacks[key].push_back([cb = std::move(callback)](const void* event) {
            cb(*static_cast<const TEvent*>(event));
        });
    }

    template<typename TEvent>
    void publish(const TEvent& event)
    {
        auto key = std::type_index(typeid(TEvent));
        auto it  = m_subscriberCallbacks.find(key);

        if (it == m_subscriberCallbacks.end()) return;

        for (const auto& callback : it->second) // run each callback function
            callback(&event);
    }

private:
    std::unordered_map<
        std::type_index,
        std::vector<std::function<void(const void*)>>
    > m_subscriberCallbacks;
};
