#pragma once
#include "UI/UIComponent.hpp"
#include "Components/CaptureManager.hpp"

namespace IWXMVM::UI
{
    class CaptureMenu : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        std::optional<int32_t> GetDisplayPassIndex() const;
        void ResetPassPreview();

       private:
        void Initialize() final;

        void DrawStreamsSection(Components::CaptureSettings& captureSettings);
    };
}  // namespace IWXMVM::UI