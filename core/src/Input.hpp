#pragma once
#include "StdInclude.hpp"

namespace IWXMVM::Input
{
	bool KeyDown(ImGuiKey key);
	bool KeyUp(ImGuiKey key);
	bool KeyHeld(ImGuiKey key);
	bool MouseButtonHeld(ImGuiMouseButton mouseButton);

	Vector2 GetMouseDelta();
	float GetScrollDelta();

	float GetDeltaTime();
}