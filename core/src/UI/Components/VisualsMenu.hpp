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

        std::string OpenFileDialog();
        void LoadConfig(std::string);

        Visuals visuals;
        bool visualsInitialized = false;
    };
}  // namespace IWXMVM::UI