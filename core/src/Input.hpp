#pragma once
#include "StdInclude.hpp"

#include "Configuration/InputConfiguration.hpp"

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

        static ImVec2 GetMouseDelta();
        static float GetScrollDelta();
        static void UpdateState(ImGuiIO& io);

        static float GetDeltaTime();

       private:
        static inline float mouseWheelDelta;
    };
}  // namespace IWXMVM