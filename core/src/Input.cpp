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

	bool Input::BindHeld(std::string_view bindName) {
		KeyBind bind = GetKeyBind(bindName);
		if (std::holds_alternative<ImGuiKey>(bind)) {
			return KeyHeld(std::get<ImGuiKey>(bind));
		}
		else if (std::holds_alternative<ImGuiMouseButton_>(bind)) {
			return MouseButtonHeld(std::get<ImGuiMouseButton_>(bind));
		}
		return false; //Ivalid Keybind
	}

	void Input::AddKeyBind(std::string_view actionName, KeyBind key) {
		keyBinds[actionName.data()] = key;
	}

	KeyBind Input::GetKeyBind(std::string_view actionName) {
		auto it = keyBinds.find(actionName.data());
		if (it != keyBinds.end())
		{
			return it->second;
		}
		return KeyBind(); // KeyBind invalide
	}

	void Input::LoadDefaultConfig() {

		//FreeCamera Binds

		AddKeyBind("freeCameraForward", ImGuiKey_W);
		AddKeyBind("freeCameraBackward", ImGuiKey_S);
		AddKeyBind("freeCameraLeft", ImGuiKey_A);
		AddKeyBind("freeCameraRight", ImGuiKey_D);
		AddKeyBind("freeCameraUp", ImGuiKey_E);
		AddKeyBind("freeCameraDown", ImGuiKey_Q);
		AddKeyBind("freeCameraReset", ImGuiMouseButton_Middle);

		//OrbitCamera Binds

		AddKeyBind("orbitCameraReset", ImGuiKey_F4);
		AddKeyBind("orbitCameraRotate", ImGuiMouseButton_Middle);
		AddKeyBind("orbitCameraMove", ImGuiMouseButton_Right);
	}

	void Input::LoadConfig(std::string_view path) {
		std::ifstream configFile(path.data());
		if (!configFile.is_open()) {
			LoadDefaultConfig();
			return;
		}

		std::string actionName;
		std::string keyType;
		int keyValue;

		while (configFile >> actionName >> keyType >> keyValue) {
			LOG_INFO("{}", actionName);
			KeyBind key;
			if (keyType == "Key") {
				key = static_cast<ImGuiKey>(keyValue);
			}
			else if (keyType == "MouseButton") {
				key = static_cast<ImGuiMouseButton_>(keyValue);
			}
			else {
				throw std::exception("LoadConfig can't read KeyType in config file");
			}

			AddKeyBind(actionName, key);
		}

		configFile.close();
	}


	void Input::WriteConfig(std::ofstream& configFile) {
		LOG_INFO("Writing Config File");
		for (const auto& entry : keyBinds) {
			if (std::holds_alternative<ImGuiKey>(entry.second)) {
				configFile << entry.first << " Key " << static_cast<int>(std::get<ImGuiKey>(entry.second)) << "\n";

			}
			else if (std::holds_alternative<ImGuiMouseButton_>(entry.second)) {
				configFile << entry.first << " MouseButton " << static_cast<int>(std::get<ImGuiMouseButton_>(entry.second)) << "\n";
			}
		}

		configFile.close();
	}

	void Input::SaveConfig(std::string_view path) {
		LOG_INFO("Saving Config Attempt");
		std::ofstream configFile(path.data());
		if (!configFile.is_open()) {
			throw std::exception("SaveConfig can't open config file");
			return;
		}

		// Write config file
		WriteConfig(configFile);
	}

}