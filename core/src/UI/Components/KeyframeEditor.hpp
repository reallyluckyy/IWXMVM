#pragma once
#include "UI/UIComponent.hpp"
#include "Types/KeyframeableProperty.hpp"
#include "Types/Keyframe.hpp"

namespace IWXMVM::UI
{
    class KeyframeEditor : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        std::tuple<int32_t, int32_t> GetDisplayTickRange() const;

       private:
        void Initialize() final;

        void HandleTimelineZoomInteractions(float barLength);

        void DrawCurveEditorInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                     uint32_t displayStartTick, uint32_t displayEndTick, const float width,
                                     std::vector<Types::Keyframe>& keyframes, int32_t keyframeValueIndex,
                                     std::function<float(const Types::Keyframe&)> GetKeyframeValue,
                                     std::function<void(Types::Keyframe&, float)> SetKeyframeValue);

        void DrawCurveEditor(const Types::KeyframeableProperty& property, const auto width);

        void DrawKeyframeSliderInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                        uint32_t displayStartTick, uint32_t displayEndTick, std::vector<Types::Keyframe>& keyframes);
        void DrawKeyframeSlider(const Types::KeyframeableProperty& property);

        int32_t displayStartTick, displayEndTick;
    };
}  // namespace IWXMVM::UI