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

       private:
        PreferencesConfiguration();
    };
}  // namespace IWXMVM