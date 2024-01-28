#include "StdInclude.hpp"
#include "PreferencesConfiguration.hpp"

namespace IWXMVM
{
    PreferencesConfiguration::PreferencesConfiguration()
    {
    }

    void PreferencesConfiguration::Deserialize(const nlohmann::json& j)
    {
        if (!j.is_object())
        {
            throw std::runtime_error("Failed to deserialize preferences configuration: root node is not an object");
        }

        showKeybindHints = j[NODE_SHOW_KEYBIND_HINTS].get<bool>();
        freecamSpeed = j[NODE_FREECAM_SPEED].get<float>();
        freecamMouseSpeed = j[NODE_FREECAM_MOUSE_SPEED].get<float>();
        orbitRotationSpeed = j[NODE_ORBIT_ROTATION_SPEED].get<float>();
        orbitMoveSpeed = j[NODE_ORBIT_MOVE_SPEED].get<float>();
        orbitZoomSpeed = j[NODE_ORBIT_ZOOM_SPEED].get<float>();
    }

    void PreferencesConfiguration::Serialize(nlohmann::json& j) const
    {
        j[NODE_SHOW_KEYBIND_HINTS] = showKeybindHints;
        j[NODE_FREECAM_SPEED] = freecamSpeed;
        j[NODE_FREECAM_MOUSE_SPEED] = freecamMouseSpeed;
        j[NODE_ORBIT_ROTATION_SPEED] = orbitRotationSpeed;
        j[NODE_ORBIT_MOVE_SPEED] = orbitMoveSpeed;
        j[NODE_ORBIT_ZOOM_SPEED] = orbitZoomSpeed;
    }
}  // namespace IWXMVM