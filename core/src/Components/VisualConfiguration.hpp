#pragma once
#include "Types/Filmtweaks.hpp"
#include "Types/Dof.hpp"
#include "Types/HudInfo.hpp"

namespace IWXMVM::Components
{
    class VisualConfiguration
    {
       public:
        struct Settings
        {
            // DOF
            Types::DoF dof;

            // SUN
            glm::vec3 sunColorUI;
            glm::vec3 sunDirectionUI;
            float sunPitch;
            float sunYaw;
            float sunBrightness = 1;

            // FILMTWEAKS
            Types::Filmtweaks filmtweaks;

            // MISC
            Types::HudInfo hudInfo;
        };

        static std::optional<Settings> Load(std::filesystem::path file);
        static void Save(std::filesystem::path file, Settings settings);
    };
}