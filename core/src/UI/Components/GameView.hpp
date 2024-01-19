#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class GameView : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        ImVec2 GetViewportPosition()
        {
            return viewportPosition;
        }

        ImVec2 GetViewportSize()
        {
            return viewportSize;
        }

       private:
        void Initialize() final;
        void DrawTopBar();

        void LockMouse();

        void DrawGizmoControls();

        IDirect3DTexture9* texture = NULL;
        ImVec2 textureSize = ImVec2(0, 0);

        ImVec2 viewportPosition = ImVec2(0, 0);
        ImVec2 viewportSize = ImVec2(0, 0);
    };
}  // namespace IWXMVM::UI