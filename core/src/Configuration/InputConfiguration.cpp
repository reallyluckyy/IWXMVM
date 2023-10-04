#include "StdInclude.hpp"
#include "InputConfiguration.hpp"

namespace IWXMVM
{
    struct Key
    {
        ImGuiKey imGuiKey;
        std::string_view string;
    };

    // clang-format off

    // Default bindings
    // This has to match the order of the enum in "InputConfiguration.hpp"

    Key bindToKeyMap[Bind::Count] = {
        {ImGuiKey_K,           "DollyAddNode"},
        {ImGuiKey_L,           "DollyClearNodes"},
        {ImGuiKey_J,           "DollyPlayPath"},
        {ImGuiKey_S,           "FreeCameraBackward"},
        {ImGuiKey_Q,           "FreeCameraDown"},
        {ImGuiKey_W,           "FreeCameraForward"},
        {ImGuiKey_A,           "FreeCameraLeft"},
        {ImGuiKey_MouseMiddle, "FreeCameraReset"},
        {ImGuiKey_D,           "FreeCameraRight"},
        {ImGuiKey_E,           "FreeCameraUp"},
        {ImGuiKey_MouseRight,  "OrbitCameraMove"},
        {ImGuiKey_F4,          "OrbitCameraReset"},
        {ImGuiKey_MouseMiddle, "OrbitCameraRotate"},
        {ImGuiKey_UpArrow,     "PlaybackFaster"},
        {ImGuiKey_LeftArrow,   "PlaybackSkipBackward"},
        {ImGuiKey_RightArrow,  "PlaybackSkipForward"},
        {ImGuiKey_DownArrow,   "PlaybackSlower"},
        {ImGuiKey_Space,       "PlaybackToggle"},
    };

    // clang-format on

    std::unordered_map<std::string, Bind> stringToBindMap;

    void InitializeStringToBindMap()
    {
        for (std::size_t i = 0; i < (std::size_t)Bind::Count; i++)
        {
            stringToBindMap[(std::string)bindToKeyMap[i].string] = (Bind)i;
        }
    }

    void BindsDeserialize(const nlohmann::json& j)
    {
        if (stringToBindMap.empty())
        {
            InitializeStringToBindMap();
        }

        for (const auto& entry : j)
        {
            if (!entry.is_object())
            {
                LOG_ERROR("Invalid keybind entry found! (not an object)");
                return;
            }

            std::string action = entry.at(NODE_ACTION).get<std::string>();
            auto rawKeyValue = entry.at(NODE_KEY).get<std::int32_t>();

            if (stringToBindMap.contains(action))
            {
                auto bind = stringToBindMap[action];
                bindToKeyMap[bind].imGuiKey = (ImGuiKey)rawKeyValue;
            }
        }
    }

    void BindsSerialize(nlohmann::json& j)
    {
        j = nlohmann::json::array();
        for (std::size_t i = 0; i < (std::size_t)Bind::Count; i++)
        {
            Bind bind = (Bind)i;

            nlohmann::json entry;
            entry[NODE_ACTION] = BindToString(bind);
            entry[NODE_KEY] = (int32_t)BindToKey(bind);

            j.push_back(entry);
        }
    }

    ImGuiKey BindToKey(Bind bind) noexcept
    {
        return bindToKeyMap[bind].imGuiKey;
    }

    Bind BindFromString(std::string_view string)
    {
        return Bind::Count;
    }

    std::string_view BindToString(Bind bind) noexcept
    {
        return bindToKeyMap[bind].string;
    }
}  // namespace IWXMVM