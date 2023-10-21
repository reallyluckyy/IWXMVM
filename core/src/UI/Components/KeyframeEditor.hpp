#pragma once
#include "UI/UIComponent.hpp"
#include "Types/KeyframeableProperty.hpp"

namespace IWXMVM::UI
{
    class KeyframeEditor : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

       private:
        void Initialize() final;
        void DrawKeyframeSlider(const Types::KeyframeableProperty& property);
        void DrawCurveEditor(const Types::KeyframeableProperty& property);
    };
}  // namespace IWXMVM::UI