#include "StdInclude.hpp"
#include "Events.hpp"

namespace IWXMVM::Events
{
    std::map<EventType, std::vector<std::function<void()>>> listeners;

    void Invoke(const EventType eventType)
    {
        for (const auto& function : listeners[eventType])
        {
            function();
        }
    }

    void RegisterListener(const EventType eventType, const std::function<void()> function)
    {
        listeners[eventType].push_back(function);
    }
}  // namespace IWXMVM::Events