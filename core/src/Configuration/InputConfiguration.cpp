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
        /*
        keyBinds = {
                {BIND_FREE_CAMERA_FORWARD, ImGuiKey_W},
                {BIND_FREE_CAMERA_BACKWARD, ImGuiKey_S},
                {BIND_FREE_CAMERA_LEFT, ImGuiKey_A},
                {BIND_FREE_CAMERA_RIGHT, ImGuiKey_D},
                {BIND_FREE_CAMERA_UP, ImGuiKey_E},
                {BIND_FREE_CAMERA_DOWN, ImGuiKey_Q},
                {BIND_FREE_CAMERA_RESET, ImGuiMouseButton_Middle},

                {BIND_ORBIT_CAMERA_RESET, ImGuiKey_F4},
                {BIND_ORBIT_CAMERA_ROTATE, ImGuiMouseButton_Middle},
                {BIND_ORBIT_CAMERA_MOVE, ImGuiMouseButton_Right},

                {BIND_DOLLY_ADD_MARKER, ImGuiKey_K},
                {BIND_DOLLY_CLEAR_MARKERS, ImGuiKey_L},
                {BIND_DOLLY_PLAY_PATH, ImGuiKey_J},

                {BIND_PLAYBACK_TOGGLE, ImGuiKey_Space},
                {BIND_PLAYBACK_FASTER, ImGuiKey_UpArrow},
                {BIND_PLAYBACK_SLOWER, ImGuiKey_DownArrow},
                {BIND_PLAYBACK_SKIP_FORWARD, ImGuiKey_RightArrow},
                {BIND_PLAYBACK_SKIP_BACKWARD, ImGuiKey_LeftArrow},

        };
        */
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

            std::string action = entry.at(NODE_ACTION).get<std::string>();
            auto rawKeyValue = entry.at(NODE_KEY).get<std::int32_t>();
            Key key = ParseRawKeyValue(rawKeyValue);
            keyBinds[action] = key;

            LOG_DEBUG("Deserialized keybind for action {} with key {}", action, rawKeyValue);
        }
    }
}  // namespace IWXMVM