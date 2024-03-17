#include "StdInclude.hpp"
#include "InputConfiguration.hpp"

namespace IWXMVM
{
    InputConfiguration::InputConfiguration()
        :
        actionToBindMap([]()
        {
            std::array<Bind, Action::Count> defaults{};

            defaults[Action::DollyAddNode]         = Bind{ImGuiKey_K, "DollyAddNode"};
            defaults[Action::DollyClearNodes]      = Bind{ImGuiKey_L, "DollyClearNodes"};
            defaults[Action::DollyPlayPath]        = Bind{ImGuiKey_J, "DollyPlayPath"};
            defaults[Action::FreeCameraActivate]   = Bind{ImGuiKey_F, "FreeCameraActivate"};
            defaults[Action::FreeCameraForward]    = Bind{ImGuiKey_W, "FreeCameraForward"};
            defaults[Action::FreeCameraBackward]   = Bind{ImGuiKey_S, "FreeCameraBackward"};
            defaults[Action::FreeCameraLeft]       = Bind{ImGuiKey_A, "FreeCameraLeft"};
            defaults[Action::FreeCameraRight]      = Bind{ImGuiKey_D, "FreeCameraRight"};
            defaults[Action::FreeCameraUp]         = Bind{ImGuiKey_E, "FreeCameraUp"};
            defaults[Action::FreeCameraDown]       = Bind{ImGuiKey_Q, "FreeCameraDown"};
            defaults[Action::FreeCameraFaster]     = Bind{ImGuiKey_LeftShift, "FreeCameraFaster"};
            defaults[Action::FreeCameraSlower]     = Bind{ImGuiKey_LeftCtrl, "FreeCameraSlower"};
            defaults[Action::FreeCameraReset]      = Bind{ImGuiKey_MouseMiddle, "FreeCameraReset"};
            defaults[Action::OrbitCameraReset]     = Bind{ImGuiKey_F4, "OrbitCameraReset"};
            defaults[Action::OrbitCameraRotate]    = Bind{ImGuiKey_MouseRight, "OrbitCameraRotate"};
            defaults[Action::PlaybackFaster]       = Bind{ImGuiKey_UpArrow, "PlaybackFaster"};
            defaults[Action::PlaybackSlower]       = Bind{ImGuiKey_DownArrow, "PlaybackSlower"};
            defaults[Action::PlaybackSkipBackward] = Bind{ImGuiKey_LeftArrow, "PlaybackSkipBackward"};
            defaults[Action::PlaybackSkipForward]  = Bind{ImGuiKey_RightArrow, "PlaybackSkipForward"};
            defaults[Action::PlaybackToggle]       = Bind{ImGuiKey_Space, "PlaybackToggle"};
            defaults[Action::TimeFrameMoveStart]   = Bind{ImGuiKey_B, "TimeFrameMoveStart"};
            defaults[Action::TimeFrameMoveEnd]     = Bind{ImGuiKey_N, "TimeFrameMoveEnd"};
            defaults[Action::FirstPersonToggle]     = Bind{ImGuiKey_F2, "FirstPersonToggle"};
            return defaults;
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

            for (std::size_t i = 0; i < (std::size_t)Action::Count; i++)
            {
                const auto& bind = actionToBindMap[i];
                if (bind.actionName == actionName)
                {
                    actionToBindMap[i].key = (ImGuiKey)rawKeyValue;
                }
            }
        }
    }

    void InputConfiguration::Serialize(nlohmann::json& j) const
    {
        j = nlohmann::json::array();
        for (std::size_t i = 0; i < (std::size_t)Action::Count; i++)
        {
            Action action = (Action)i;

            nlohmann::json entry;
            entry[NODE_ACTION] = ActionToString(action);
            entry[NODE_KEY] = (int32_t)GetBoundKey(action);

            j.push_back(entry);
        }
    }

    ImGuiKey InputConfiguration::GetBoundKey(Action action) const noexcept
    {
        return actionToBindMap[action].key;
    }

    void InputConfiguration::SetKeyBinding(Action action, ImGuiKey key) noexcept
    {
        actionToBindMap[action].key = key;
        Configuration::Get().Write();
    }

    std::string_view InputConfiguration::ActionToString(Action action) const noexcept
    {
        return actionToBindMap[action].actionName;
    }
}  // namespace IWXMVM