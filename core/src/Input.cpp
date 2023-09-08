#include "StdInclude.hpp"
#include "Input.hpp"

namespace IWXMVM
{
	bool Input::KeyDown(ImGuiKey key)
	{
		return ImGui::IsKeyPressed(key);
	}

	bool Input::KeyUp(ImGuiKey key)
	{
		return ImGui::IsKeyReleased(key);
	}

	bool Input::KeyHeld(ImGuiKey key)
	{
		return ImGui::IsKeyDown(key);
	}

	bool Input::MouseButtonHeld(ImGuiMouseButton mouseButton)
	{
		return ImGui::IsMouseDown(mouseButton);
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

	bool Input::BindHeld(std::string_view bindName) 
	{
		Key bind = InputConfiguration::Get().GetKeyBind(bindName);
		if (std::holds_alternative<ImGuiKey>(bind)) 
		{
			return KeyHeld(std::get<ImGuiKey>(bind));
		}
		else if (std::holds_alternative<ImGuiMouseButton_>(bind)) 
		{
			return MouseButtonHeld(std::get<ImGuiMouseButton_>(bind));
		}
		return false; // Invalid bind
	}
}