#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class KeyframeEditor : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

       private:
        void Initialize() final;
        void DrawKeyframeSlider(const char* label);
    };
}  // namespace IWXMVM::UI