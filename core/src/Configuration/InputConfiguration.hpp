#pragma once
#include "Configuration.hpp"

namespace IWXMVM
{
    enum Action
    {
        DollyAddNode,
        DollyClearNodes,
        DollyPlayPath,
        FreeCameraBackward,
        FreeCameraDown,
        FreeCameraForward,
        FreeCameraLeft,
        FreeCameraReset,
        FreeCameraRight,
        FreeCameraUp,
        OrbitCameraMove,
        OrbitCameraReset,
        OrbitCameraRotate,
        PlaybackFaster,
        PlaybackSkipBackward,
        PlaybackSkipForward,
        PlaybackSlower,
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

        ImGuiKey GetBoundKey(Action bind) const noexcept;
        std::string_view ActionToString(Action bind) const noexcept;

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