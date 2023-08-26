#pragma once
#include "StdInclude.hpp"

namespace IWXMVM
{
	class Input 
	{	
	public:
		static bool KeyDown(ImGuiKey key);
		static bool KeyUp(ImGuiKey key);
		static bool KeyHeld(ImGuiKey key);
		static bool MouseButtonHeld(ImGuiMouseButton mouseButton);

		static glm::vec2 GetMouseDelta();
		static float GetScrollDelta();
		static void UpdateState(ImGuiIO& io);

		static float GetDeltaTime();

	private:
		static inline float mouseWheelDelta;
	};
}