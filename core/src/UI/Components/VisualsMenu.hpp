#pragma once
#include "UI/UIComponent.hpp"
#include "glm/vec3.hpp"
#include "Types/dof.hpp"

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
        };

        void Initialize() final;
        void RenderDOF();
        void RenderSun();

        void UpdateDof();
        void UpdateSun();
        void UpdateSunAngle();

        Visuals visuals;
        bool visualsInitialized = false;
    };
}  // namespace IWXMVM::UI