#include "StdInclude.hpp"
#include "InputConfiguration.hpp"

namespace IWXMVM
{
	Key InputConfiguration::GetKeyBind(std::string_view actionName) const
	{
		auto it = keyBinds.find(actionName.data());
		if (it != keyBinds.end())
			return it->second;
		else
			return ImGuiKey();
	}

	void InputConfiguration::AddKeyBind(std::string_view actionName, Key key)
	{
		keyBinds[actionName.data()] = key;
	}

	void InputConfiguration::LoadDefault()
	{
		keyBinds = {
			{BIND_FREE_CAMERA_FORWARD, ImGuiKey_W},
			{BIND_FREE_CAMERA_BACKWARD, ImGuiKey_S},
			{BIND_FREE_CAMERA_LEFT, ImGuiKey_A},
			{BIND_FREE_CAMERA_RIGHT, ImGuiKey_D},
			{BIND_FREE_CAMERA_UP, ImGuiKey_E},
			{BIND_FREE_CAMERA_DOWN, ImGuiKey_Q},
			{BIND_FREE_CAMERA_RESET, ImGuiKey_MouseMiddle},

			{BIND_ORBIT_CAMERA_RESET, ImGuiKey_F4},
			{BIND_ORBIT_CAMERA_ROTATE, ImGuiKey_MouseMiddle},
			{BIND_ORBIT_CAMERA_MOVE, ImGuiKey_MouseRight},

			{BIND_DOLLY_ADD_MARKER, ImGuiKey_K},
			{BIND_DOLLY_CLEAR_MARKERS, ImGuiKey_L},
			{BIND_DOLLY_PLAY_PATH, ImGuiKey_J},

			{BIND_PLAYBACK_TOGGLE, ImGuiKey_Space},
			{BIND_PLAYBACK_FASTER, ImGuiKey_UpArrow},
			{BIND_PLAYBACK_SLOWER, ImGuiKey_DownArrow},
			{BIND_PLAYBACK_SKIP_FORWARD, ImGuiKey_RightArrow},
			{BIND_PLAYBACK_SKIP_BACKWARD, ImGuiKey_LeftArrow},

		};
	}

	void InputConfiguration::Serialize(json& j)
	{
		j = json::array();
		for (auto bind : keyBinds)
		{
			json entry;
			entry[NODE_ACTION] = bind.first;
			entry[NODE_KEY] = (int32_t)bind.second;

			j.push_back(entry);
		}
	}

	void InputConfiguration::Deserialize(const json& j)
	{
		keyBinds.clear();

		for (auto entry : j)
		{
			if (!entry.is_object())
			{
				LOG_ERROR("Invalid keybind entry found! (not an object)");
				return;
			}

			std::string	action = entry.at(NODE_ACTION).get<std::string>();
			auto rawKeyValue = entry.at(NODE_KEY).get<std::int32_t>();
			Key key = (Key)rawKeyValue;
			keyBinds[action] = key;

			LOG_DEBUG("Deserialized keybind for action {} with key {}", action, rawKeyValue);
		}
	}
}