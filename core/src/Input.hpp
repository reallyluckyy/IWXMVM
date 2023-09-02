#pragma once
#include "StdInclude.hpp"

namespace IWXMVM
{
	typedef std::variant<ImGuiKey, ImGuiMouseButton_> KeyBind;
	static std::unordered_map<std::string, KeyBind> keyBinds;

	class Input
	{
	public:
		static bool KeyDown(ImGuiKey key);
		static bool KeyUp(ImGuiKey key);
		static bool KeyHeld(ImGuiKey key);
		static bool MouseButtonHeld(ImGuiMouseButton mouseButton);
		static bool BindHeld(std::string_view bindName);

		static glm::vec2 GetMouseDelta();
		static float GetScrollDelta();
		static void UpdateState(ImGuiIO& io);

		static float GetDeltaTime();

		static void AddKeyBind(std::string_view actionName, KeyBind key);

		static KeyBind GetKeyBind(std::string_view actionName);

		static void LoadDefaultConfig();

		static void LoadConfig(std::string_view path);
		static void SaveConfig(std::string_view path);

		static void WriteConfig(std::ofstream& configFile);

	private:
		static inline float mouseWheelDelta;

	};
}