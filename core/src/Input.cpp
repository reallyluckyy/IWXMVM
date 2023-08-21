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

	Vector2 Input::GetMouseDelta()
	{
		ImGuiIO& io = ImGui::GetIO();
		return Vector2(io.MouseDelta.x, io.MouseDelta.y);
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
}