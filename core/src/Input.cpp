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

	Vector2 GetMouseDelta()
	{
		ImGuiIO& io = ImGui::GetIO();
		return Vector2(io.MouseDelta.x, io.MouseDelta.y);
	}

	float GetDeltaTime()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.DeltaTime;
	}
}