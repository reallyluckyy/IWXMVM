#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class CameraMenu : public UIComponent
    {
       public:
        void Initialize() final;
        void Render() final;
        void Release() final;

       private:
        void RenderHotkeyOverlay();
    };
}  // namespace IWXMVM::UI