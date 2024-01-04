#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class GameView : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        int32_t GetHoveredCampathNodeId()
        {
            return hoveredCampathNodeId;
        }

        int32_t GetSelectedCampathNodeId()
        {
            return selectedCampathNodeId;
        }

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

        void HandleCampathNodeInteraction();

        IDirect3DTexture9* texture = NULL;
        ImVec2 textureSize = ImVec2(0, 0);

        int32_t hoveredCampathNodeId = -1;
        int32_t selectedCampathNodeId = -1;
        int32_t selectedCampathNodeHandle = -1;

        ImVec2 viewportPosition = ImVec2(0, 0);
        ImVec2 viewportSize = ImVec2(0, 0);
    };
}  // namespace IWXMVM::UI