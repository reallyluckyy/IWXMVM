#pragma once

namespace IWXMVM
{
    enum class EventType
    {
        OnFrame, // once per rendered frame
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