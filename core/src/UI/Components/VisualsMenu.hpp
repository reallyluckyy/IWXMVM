#pragma once
#include "UI/UIComponent.hpp"
#include "glm/vec3.hpp"
#include "Types/dof.hpp"
#include "Types/Filmtweaks.hpp"
#include "Components/VisualConfiguration.hpp"

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

        void LoadPreset(Preset);
        void AddPresetToRecent(Preset);

        Components::VisualConfiguration::Settings visuals;
        Components::VisualConfiguration::Settings defaultVisuals;
        bool visualsInitialized = false;
        Preset defaultPreset;
        Preset currentPreset;
    };
}  // namespace IWXMVM::UI