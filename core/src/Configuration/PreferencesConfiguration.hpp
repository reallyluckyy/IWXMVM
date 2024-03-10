#pragma once
#include "Configuration.hpp"

namespace IWXMVM
{
    class PreferencesConfiguration
    {
       public:
        static PreferencesConfiguration& Get()
        {
            static PreferencesConfiguration instance;
            return instance;
        }

        PreferencesConfiguration(PreferencesConfiguration const&) = delete;
        void operator=(PreferencesConfiguration const&) = delete;

        void Deserialize(const nlohmann::json& j);
        void Serialize(nlohmann::json& j) const;


        bool showKeybindHints = true;

        float freecamSpeed = 300.0f;
        float freecamMouseSpeed = 0.1f;

        float orbitRotationSpeed = 0.15f;
        float orbitMoveSpeed = 0.3f;
        float orbitZoomSpeed = 0.8f;

        std::filesystem::path captureOutputDirectory = std::filesystem::path();

       private:
        PreferencesConfiguration();

        const std::string_view NODE_SHOW_KEYBIND_HINTS = "showKeybindHints";
        const std::string_view NODE_FREECAM_SPEED = "freecamSpeed";
        const std::string_view NODE_FREECAM_MOUSE_SPEED = "freecamMouseSpeed";
        const std::string_view NODE_ORBIT_ROTATION_SPEED = "orbitRotationSpeed";
        const std::string_view NODE_ORBIT_MOVE_SPEED = "orbitMoveSpeed";
        const std::string_view NODE_ORBIT_ZOOM_SPEED = "orbitZoomSpeed";
        const std::string_view NODE_CAPTURE_OUTPUT_DIRECTORY = "captureOutputDirectory";

    };
}  // namespace IWXMVM