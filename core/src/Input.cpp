#include "StdInclude.hpp"
#include "Input.hpp"

namespace IWXMVM
{
    const std::map<ImGuiKey, ImGuiMouseButton> mouseButtonMap = {{ImGuiKey_MouseLeft, ImGuiMouseButton_Left},
                                                                 {ImGuiKey_MouseRight, ImGuiMouseButton_Right},
                                                                 {ImGuiKey_MouseMiddle, ImGuiMouseButton_Middle}};

    bool IsMouseButton(ImGuiKey key)
    {
        return key >= ImGuiKey_MouseLeft && key <= ImGuiKey_MouseWheelY;
    }

    bool Input::KeyDown(ImGuiKey key)
    {
        if (IsMouseButton(key))
            return ImGui::IsMouseClicked(mouseButtonMap.at(key));
        return ImGui::IsKeyPressed(key);
    }

    bool Input::KeyUp(ImGuiKey key)
    {
        if (IsMouseButton(key))
            return ImGui::IsMouseReleased(mouseButtonMap.at(key));
        return ImGui::IsKeyReleased(key);
    }

    bool Input::KeyHeld(ImGuiKey key)
    {
        if (IsMouseButton(key))
            return ImGui::IsMouseDown(mouseButtonMap.at(key));
        return ImGui::IsKeyDown(key);
    }

    glm::vec2 Input::GetMouseDelta()
    {
        ImGuiIO& io = ImGui::GetIO();
        return glm::vec2(io.MouseDelta.x, io.MouseDelta.y);
    }

    float Input::GetScrollDelta()
    {
        return mouseWheelDelta;
    }

    void Input::UpdateState(ImGuiIO& io)
    {
        mouseWheelDelta = io.MouseWheel;
    }

    float Input::GetDeltaTime()
    {
        ImGuiIO& io = ImGui::GetIO();
        return io.DeltaTime;
    }

    bool Input::BindHeld(Bind bind)
    {
        return Input::KeyHeld(BindToKey(bind));
    }

    bool Input::BindDown(Bind bind)
    {
        return Input::KeyDown(BindToKey(bind));
    }
}  // namespace IWXMVM