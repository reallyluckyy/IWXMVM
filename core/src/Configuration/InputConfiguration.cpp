#include "StdInclude.hpp"
#include "InputConfiguration.hpp"

namespace IWXMVM
{
    InputConfiguration::InputConfiguration()
        :
        actionToBindMap([]()
        {
            std::array<Bind, Action::Count> tmp{};

            tmp[Action::DollyAddNode]         = Bind{ImGuiKey_K, "DollyAddNode"};
            tmp[Action::DollyClearNodes]      = Bind{ImGuiKey_L, "DollyClearNodes"};
            tmp[Action::DollyPlayPath]        = Bind{ImGuiKey_J, "DollyPlayPath"};
            tmp[Action::FreeCameraBackward]   = Bind{ImGuiKey_S, "FreeCameraBackward"};
            tmp[Action::FreeCameraDown]       = Bind{ImGuiKey_Q, "FreeCameraDown"};
            tmp[Action::FreeCameraForward]    = Bind{ImGuiKey_W, "FreeCameraForward"};
            tmp[Action::FreeCameraLeft]       = Bind{ImGuiKey_A, "FreeCameraLeft"};
            tmp[Action::FreeCameraReset]      = Bind{ImGuiKey_MouseMiddle, "FreeCameraReset"};
            tmp[Action::FreeCameraRight]      = Bind{ImGuiKey_D, "FreeCameraRight"};
            tmp[Action::FreeCameraUp]         = Bind{ImGuiKey_E, "FreeCameraUp"};
            tmp[Action::OrbitCameraMove]      = Bind{ImGuiKey_MouseRight, "OrbitCameraMove"};
            tmp[Action::OrbitCameraReset]     = Bind{ImGuiKey_F4, "OrbitCameraReset"};
            tmp[Action::OrbitCameraRotate]    = Bind{ImGuiKey_MouseMiddle, "OrbitCameraRotate"};
            tmp[Action::PlaybackFaster]       = Bind{ImGuiKey_UpArrow, "PlaybackFaster"};
            tmp[Action::PlaybackSkipBackward] = Bind{ImGuiKey_LeftArrow, "PlaybackSkipBackward"};
            tmp[Action::PlaybackSkipForward]  = Bind{ImGuiKey_RightArrow, "PlaybackSkipForward"};
            tmp[Action::PlaybackSlower]       = Bind{ImGuiKey_DownArrow, "PlaybackSlower"};
            tmp[Action::PlaybackToggle]       = Bind{ImGuiKey_Space, "PlaybackToggle"};

            return tmp;
        }()),
        stringToActionMap([&]()
        {
            std::unordered_map<std::string, Action> tmp{};

            for (std::size_t i = 0; i < actionToBindMap.size(); i++)
            {
                std::string name(actionToBindMap[i].name);
                tmp[name] = (Action)i;
            }

            return tmp;
        }())
    {
    }

    void InputConfiguration::Deserialize(const nlohmann::json& j)
    {
        for (const auto& entry : j)
        {
            if (!entry.is_object())
            {
                LOG_ERROR("Invalid keybind entry found! (not an object)");
                return;
            }

            std::string actionName = entry.at(NODE_ACTION).get<std::string>();
            auto rawKeyValue = entry.at(NODE_KEY).get<std::int32_t>();

            if (stringToActionMap.contains(actionName))
            {
                auto action = stringToActionMap.at(actionName);
                actionToBindMap[action].key = (ImGuiKey)rawKeyValue;
            }
        }
    }

    void InputConfiguration::Serialize(nlohmann::json& j)
    {
        j = nlohmann::json::array();
        for (std::size_t i = 0; i < (std::size_t)Action::Count; i++)
        {
            Action action = (Action)i;

            nlohmann::json entry;
            entry[NODE_ACTION] = ActionToString(action);
            entry[NODE_KEY] = (int32_t)ActionToKey(action);

            j.push_back(entry);
        }
    }

    ImGuiKey InputConfiguration::ActionToKey(Action action) const noexcept
    {
        return actionToBindMap[action].key;
    }

    std::string_view InputConfiguration::ActionToString(Action action) const noexcept
    {
        return actionToBindMap[action].name;
    }
}  // namespace IWXMVM