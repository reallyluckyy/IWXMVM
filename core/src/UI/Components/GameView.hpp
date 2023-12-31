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

       private:
        void Initialize() final;
        void DrawTopBar();

        void LockMouse();

        bool HandleCampathNodeInteraction();

        IDirect3DTexture9* texture = NULL;
        ImVec2 textureSize = ImVec2(0, 0);

        int32_t hoveredCampathNodeId = -1;
        int32_t selectedCampathNodeId = -1;
    };
}  // namespace IWXMVM::UI