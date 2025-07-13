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
            glm::vec3 sunColor;
            glm::vec3 sunDirection;
            float sunBrightness = 1;

            // FILMTWEAKS
            Types::Filmtweaks filmtweaks;

            // MISC
            Types::HudInfo hudInfo;
        };

        static bool Load(std::filesystem::path file, Settings& visuals);
        static void Save(std::filesystem::path file, Settings settings);
    };
}