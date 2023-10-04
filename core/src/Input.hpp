#pragma once
#include "StdInclude.hpp"

#include "Configuration/InputConfiguration.hpp"

enum Action
{
    DollyAddNode,
    DollyClearNodes,
    DollyPlayPath,
    FreeCameraBackward,
    FreeCameraDown,
    FreeCameraForward,
    FreeCameraLeft,
    FreeCameraReset,
    FreeCameraRight,
    FreeCameraUp,
    OrbitCameraMove,
    OrbitCameraReset,
    OrbitCameraRotate,
    PlaybackFaster,
    PlaybackSkipBackward,
    PlaybackSkipForward,
    PlaybackSlower,
    PlaybackToggle,

    Count,
};

namespace IWXMVM
{
    class Input
    {
       public:
        static bool KeyDown(ImGuiKey key);
        static bool KeyUp(ImGuiKey key);
        static bool KeyHeld(ImGuiKey key);
        static bool BindHeld(Action action);
        static bool BindDown(Action action);

        static glm::vec2 GetMouseDelta();
        static float GetScrollDelta();
        static void UpdateState(ImGuiIO& io);

        static float GetDeltaTime();

       private:
        static inline float mouseWheelDelta;
    };
}  // namespace IWXMVM