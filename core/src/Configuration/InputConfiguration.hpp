#pragma once
#include "Configuration.hpp"

namespace IWXMVM
{
    enum Action
    {
        DollyAddNode,
        DollyClearNodes,
        DollyPlayPath,
        FreeCameraActivate,
        FreeCameraForward,
        FreeCameraBackward,
        FreeCameraLeft,
        FreeCameraRight,
        FreeCameraUp,
        FreeCameraDown,
        FreeCameraReset,
        OrbitCameraReset,
        OrbitCameraRotate,
        PlaybackFaster,
        PlaybackSlower,
        PlaybackSkipBackward,
        PlaybackSkipForward,
        PlaybackToggle,

        Count,
    };

    class InputConfiguration
    {
       public:
        static InputConfiguration& Get()
        {
            static InputConfiguration instance;
            return instance;
        }

        InputConfiguration(InputConfiguration const&) = delete;
        void operator=(InputConfiguration const&) = delete;

        void Deserialize(const nlohmann::json& j);
        void Serialize(nlohmann::json& j) const;

        ImGuiKey GetBoundKey(Action action) const noexcept;
        void SetKeyBinding(Action action, ImGuiKey key) noexcept;
        std::string_view ActionToString(Action action) const noexcept;

       private:
        InputConfiguration();

        struct Bind
        {
            ImGuiKey key;
            std::string_view actionName;
        };

        const std::string_view NODE_ACTION = "action";
        const std::string_view NODE_KEY = "key";

        std::array<Bind, Action::Count> actionToBindMap;
    };
}  // namespace IWXMVM