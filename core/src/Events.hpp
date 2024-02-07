#pragma once

namespace IWXMVM
{
    enum class EventType
    {
        OnFrame, // once per rendered frame
        OnGameFrame, // once per game frame
        OnDemoLoad,
        OnCameraChanged,
        OnRenderGameView
    };

    namespace Events
    {
        void Invoke(EventType eventType);
        void RegisterListener(EventType eventType, std::function<void()> function);
    }  // namespace Events
}  // namespace IWXMVM