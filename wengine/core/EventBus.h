#pragma once

#include <typeindex>
#include <vector>
#include <unordered_map>
#include <functional>


class EventBus
{
public:
    template<typename TEvent>
    void subscribe(std::function<void(const TEvent&)> callback)
    {
        auto key = std::type_index(typeid(TEvent));

        // add callback function to map
        m_subscriber_callbacks[key].push_back([cb = std::move(callback)](const void* event) {
            cb(*static_cast<const TEvent*>(event));
        });
    }

    template<typename TEvent>
    void publish(const TEvent& event)
    {
        auto key = std::type_index(typeid(TEvent));
        auto it = m_subscriber_callbacks.find(key);

        if (it == m_subscriber_callbacks.end()) return; // no subscribers for this event type

        for (const auto& callback : it->second)
        {
            callback(&event);
        }
    }

private:
    // store subscriber callback functions for each event type
    std::unordered_map<
        std::type_index, 
        std::vector<std::function<void(const void*)>>
    > m_subscriber_callbacks;
};