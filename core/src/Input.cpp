#include "StdInclude.hpp"
#include "Input.hpp"

namespace IWXMVM::Input
{
	bool KeyDown(ImGuiKey key)
	{
		return ImGui::IsKeyPressed(key);
	}

	bool KeyUp(ImGuiKey key)
	{
		return ImGui::IsKeyReleased(key);
	}

	bool KeyHeld(ImGuiKey key)
	{
		return ImGui::IsKeyDown(key);
	}

	bool MouseButtonHeld(ImGuiMouseButton mouseButton)
	{
		return ImGui::IsMouseDown(mouseButton);
	}

	Vector2 GetMouseDelta()
	{
		ImGuiIO& io = ImGui::GetIO();
		return Vector2(io.MouseDelta.x, io.MouseDelta.y);
	}

	float GetScrollDelta()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.MouseWheel;
	}

	float GetDeltaTime()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.DeltaTime;
	}
}