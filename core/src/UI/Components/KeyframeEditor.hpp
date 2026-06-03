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

        void SetDefaultVerticalZoom();

       private:
        void Initialize() final;

        void HandleTimelineZoomInteractions(const ImVec2 rectMin, const ImVec2 rectMax,
                                            const Types::KeyframeableProperty& property,
                                            std::optional<int32_t> valueIndex);

        bool DrawCurveEditorInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                     uint32_t displayStartTick, uint32_t displayEndTick, const float width,
                                     std::vector<Types::Keyframe>& keyframes, int32_t keyframeValueIndex, 
                                     uint32_t demoLength, std::optional<uint32_t> frozenTick);

        bool DrawCurveEditor(const Types::KeyframeableProperty& property, const auto width);

        bool DrawKeyframeSliderInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                        uint32_t displayStartTick, uint32_t displayEndTick, 
                                        std::vector<Types::Keyframe>& keyframes, uint32_t demoLength,
                                        std::optional<uint32_t> frozenTick);
        bool DrawKeyframeSlider(const Types::KeyframeableProperty& property);

        void DrawMiscButtons(ImVec2 padding, bool hasKeyframes);

        int32_t displayStartTick, displayEndTick;

        std::map<Types::KeyframeableProperty, bool> propertyVisible;
    };
}  // namespace IWXMVM::UI