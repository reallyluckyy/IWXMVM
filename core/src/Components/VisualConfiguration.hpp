#pragma once

namespace IWXMVM::Components
{
    class VisualConfiguration
    {
       public:
        struct Settings
        {
            // DOF
            bool dofActive;
            float dofFarBlur;
            float dofFarStart;
            float dofFarEnd;

            float dofNearBlur;
            float dofNearStart;
            float dofNearEnd;

            float dofBias;

            // SUN
            glm::vec3 sunColorUI;
            glm::vec3 sunDirectionUI;
            float sunPitch;
            float sunYaw;
            float sunBrightness = 1;

            // FILMTWEAKS
            bool filmtweaksActive;
            float filmtweaksBrightness;
            float filmtweaksContrast;
            float filmtweaksDesaturation;
            glm::vec3 filmtweaksTintLight;
            glm::vec3 filmtweaksTintDark;
            bool filmtweaksInvert;

            // MISC
            bool removeHud;
            bool removeHitmarker;
            bool removeScore;
            bool removeFlashbang;
            bool removeKillfeed;
            glm::vec3 team1Color;
            glm::vec3 team2Color;
        };

        static std::optional<Settings> Load(std::filesystem::path file);
        static void Save(std::filesystem::path file, Settings settings);
    };
}