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

        Configuration::ReadValueInto<bool>(j, NODE_SHOW_KEYBIND_HINTS, showKeybindHints);
        Configuration::ReadValueInto<float>(j, NODE_FREECAM_SPEED, freecamSpeed);
        Configuration::ReadValueInto<float>(j, NODE_FREECAM_MOUSE_SPEED, freecamMouseSpeed);
        Configuration::ReadValueInto<float>(j, NODE_ORBIT_ROTATION_SPEED, orbitRotationSpeed);
        Configuration::ReadValueInto<float>(j, NODE_ORBIT_MOVE_SPEED, orbitMoveSpeed);
        Configuration::ReadValueInto<float>(j, NODE_ORBIT_ZOOM_SPEED, orbitZoomSpeed);
        Configuration::ReadValueInto<std::filesystem::path>(j, NODE_CAPTURE_OUTPUT_DIRECTORY, captureOutputDirectory);
        Configuration::ReadValueInto<std::vector<std::filesystem::path>>(j, NODE_ADDITIONAL_DEMO_SEARCH_DIRECTORIES,
                                                                         additionalDemoSearchDirectories);
    }

    void PreferencesConfiguration::Serialize(nlohmann::json& j) const
    {
        j[NODE_SHOW_KEYBIND_HINTS] = showKeybindHints;
        j[NODE_FREECAM_SPEED] = freecamSpeed;
        j[NODE_FREECAM_MOUSE_SPEED] = freecamMouseSpeed;
        j[NODE_ORBIT_ROTATION_SPEED] = orbitRotationSpeed;
        j[NODE_ORBIT_MOVE_SPEED] = orbitMoveSpeed;
        j[NODE_ORBIT_ZOOM_SPEED] = orbitZoomSpeed;
        j[NODE_CAPTURE_OUTPUT_DIRECTORY] = captureOutputDirectory;
        
        j[NODE_ADDITIONAL_DEMO_SEARCH_DIRECTORIES] = nlohmann::json::array();
        for (const auto& dir : additionalDemoSearchDirectories)
        {
			j[NODE_ADDITIONAL_DEMO_SEARCH_DIRECTORIES].push_back(dir);
        }
    }
}  // namespace IWXMVM