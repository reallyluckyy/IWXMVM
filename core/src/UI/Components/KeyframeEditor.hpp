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

        void DrawKeyframeSliderInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                        uint32_t displayStartTick, uint32_t displayEndTick, uint32_t startTick,
                                        uint32_t endTick, std::vector<Types::Keyframe>& keyframes);
        void DrawKeyframeSlider(const Types::KeyframeableProperty& property);

        int32_t displayStartTick, displayEndTick;
    };
}  // namespace IWXMVM::UI