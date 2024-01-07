#pragma once
#include "UI/UIComponent.hpp"
#include "glm/vec3.hpp"
#include "Types/dof.hpp"
#include "Types/Filmtweaks.hpp"

namespace IWXMVM::UI
{
    class VisualsMenu : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

       private:
        struct Preset
        {
            std::string name;
            std::filesystem::path path;
        };
        struct Visuals
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
        std::vector<Preset> recentPresets;

        void Initialize() final;
        void RenderConfigSection();
        void RenderMiscSection();
        void RenderDOF();
        void RenderSun();
        void RenderFilmtweaks();

        void UpdateDof();
        void UpdateSun();
        void SetAngleFromPosition(glm::vec3);
        void UpdateSunAngle();
        void UpdateFilmtweaks();

        Preset OpenFileDialog();
        void LoadConfig(Preset);
        void AddPresetToRecent(Preset);


        Visuals visuals;
        Visuals defaultVisuals;
        bool visualsInitialized = false;
        Preset currentPreset;
    };
}  // namespace IWXMVM::UI