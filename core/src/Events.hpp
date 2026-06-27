#pragma once

namespace IWXMVM
{
    enum class EventType
    {
        OnFrame, // once per rendered frame
        PreDemoLoad,
        PostDemoLoad,
        OnDemoBoundsDetermined,
        OnCameraChanged,
        OnRenderGameView,
        OnRewindCompleted,
    };

    namespace Events
    {
        void Invoke(EventType eventType);
        void RegisterListener(EventType eventType, std::function<void()> function);
    }  // namespace Events
}  // namespace IWXMVM