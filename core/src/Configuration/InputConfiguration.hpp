#pragma once
#include "Configuration.hpp"

namespace IWXMVM
{
    using json = nlohmann::json;

    typedef ImGuiKey Key;

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

        const std::unordered_map<std::string, Key>& GetKeyBinds() const
        {
            return keyBinds;
        }

        Key GetKeyBind(std::string_view actionName) const;
        void AddKeyBind(std::string_view actionName, Key key);

        void Serialize(json& j);
        void Deserialize(const json& j);

        void LoadDefault();

       private:
        InputConfiguration()
        {
        }

        std::unordered_map<std::string, Key> keyBinds;

        const std::string_view NODE_ACTION = "action";
        const std::string_view NODE_KEY = "key";

       public:
        const inline static std::string_view BIND_FREE_CAMERA_FORWARD = "freeCameraForward";
        const inline static std::string_view BIND_FREE_CAMERA_BACKWARD = "freeCameraBackward";
        const inline static std::string_view BIND_FREE_CAMERA_LEFT = "freeCameraLeft";
        const inline static std::string_view BIND_FREE_CAMERA_RIGHT = "freeCameraRight";
        const inline static std::string_view BIND_FREE_CAMERA_RESET = "freeCameraReset";
        const inline static std::string_view BIND_FREE_CAMERA_UP = "freeCameraUp";
        const inline static std::string_view BIND_FREE_CAMERA_DOWN = "freeCameraDown";

        const inline static std::string_view BIND_ORBIT_CAMERA_RESET = "orbitCameraReset";
        const inline static std::string_view BIND_ORBIT_CAMERA_ROTATE = "orbitCameraRotate";
        const inline static std::string_view BIND_ORBIT_CAMERA_MOVE = "orbitCameraMove";

        const inline static std::string_view BIND_DOLLY_ADD_NODE = "dollyAddMarker";
        const inline static std::string_view BIND_DOLLY_CLEAR_NODES = "dollyClearMarkers";
        const inline static std::string_view BIND_DOLLY_PLAY_PATH = "dollyPlayPath";

        const inline static std::string_view BIND_PLAYBACK_TOGGLE = "playbackToggle";
        const inline static std::string_view BIND_PLAYBACK_FASTER = "playbackFaster";
        const inline static std::string_view BIND_PLAYBACK_SLOWER = "playbackSlower";
        const inline static std::string_view BIND_PLAYBACK_SKIP_FORWARD = "playbackSkipForward";
        const inline static std::string_view BIND_PLAYBACK_SKIP_BACKWARD = "playbackSkipBackward";
    };
}  // namespace IWXMVM