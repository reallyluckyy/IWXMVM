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
			{"freeCameraForward", ImGuiKey_W},
			{"freeCameraBackward", ImGuiKey_S},
			{"freeCameraLeft", ImGuiKey_A},
			{"freeCameraRight", ImGuiKey_D},
			{"freeCameraUp", ImGuiKey_E},
			{"freeCameraDown", ImGuiKey_Q},
			{"freeCameraReset", ImGuiMouseButton_Middle},

			{"orbitCameraReset", ImGuiKey_F4},
			{"orbitCameraRotate", ImGuiMouseButton_Middle},
			{"orbitCameraMove", ImGuiMouseButton_Right},

			{"dollyAddMarker", ImGuiKey_K},
			{"dollyClearMarkers", ImGuiKey_L},
			{"dollyPlayPath", ImGuiKey_J}
		};
	}

	int32_t GetRawKeyValue(Key key)
	{
		if (std::holds_alternative<ImGuiKey>(key))
			return std::get<ImGuiKey>(key);
		else if (std::holds_alternative<ImGuiMouseButton_>(key))
			return std::get<ImGuiMouseButton_>(key);
		else
			throw std::runtime_error("Invalid key type");
	}

	Key ParseRawKeyValue(int32_t value)
	{
		if (value > 0 && value < 512)
			return (ImGuiMouseButton_)value;
		else
			return (ImGuiKey)value;
	}

	void InputConfiguration::Serialize(json& j)
	{
		j = json::array();
		for (auto bind : keyBinds)
		{
			json entry;
			entry[NODE_ACTION] = bind.first;
			entry[NODE_KEY] = GetRawKeyValue(bind.second);

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
			Key key = ParseRawKeyValue(rawKeyValue);
			keyBinds[action] = key;

			LOG_DEBUG("Deserialized keybind for action {} with key {}", action, rawKeyValue);
		}
	}
}