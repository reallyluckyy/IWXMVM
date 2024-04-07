#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class ControlBar : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

        bool DrawDemoProgressBar(uint32_t* currentTick, uint32_t displayStartTick, uint32_t displayEndTick,
                                 uint32_t startTick, uint32_t endTick, std::optional<uint32_t> frozenTick);

       private:
        void Initialize() final;
        void DrawCaptureRangeIndicator(int32_t displayStartTick, int32_t displayEndTick, float progressBarX,
                                       float progressBarWidth, ImVec2 pauseButtonSize,
                                       uint32_t* captureSettingsTargetTick, bool* draggingTimeframe);
        void DrawCaptureRangeIndicators(int32_t displayStartTick, int32_t displayEndTick, float progressBarX,
                                        float progressBarWidth, ImVec2 pauseButtonSize);

        bool draggingStartTimeframe = false;
        bool draggingEndTimeframe = false;
    };
}  // namespace IWXMVM::UI