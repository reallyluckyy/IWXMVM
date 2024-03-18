#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "Components/CameraManager.hpp"
#include "Components/Playback.hpp"
#include "Components/Rewinding.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "UI/UIImage.hpp"
#include "UI/UIManager.hpp"
#include "Input.hpp"
#include "Components/Playback.hpp"

namespace IWXMVM::UI
{
    float playbackSpeed;

    std::optional<Types::Dvar> timescale;

    void ControlBar::Initialize()
    {
        playbackSpeed = 1.0f;
    }

    void SmartSetTickDelta(int32_t value)
    {
        // Skip forward/backward by the desired amount of ticks, while snapping to the closest capturing marker if 
        // there is one between where we are and where we want to go
        
        auto& captureSettings = Components::CaptureManager::Get().GetCaptureSettings();
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        auto targetTick = currentTick + value;
        if (value > 0)
        {
            // skipping forward
            if (captureSettings.startTick > currentTick && captureSettings.startTick < targetTick)
            {
                Components::Playback::SetTickDelta(captureSettings.startTick - currentTick);
                return;
            }
            else if (captureSettings.endTick > currentTick && captureSettings.endTick < targetTick)
            {
                Components::Playback::SetTickDelta(captureSettings.endTick - currentTick);
                return;
            }
        }
        else if (value < 0)
        {
            // skipping backward
            if (captureSettings.endTick < currentTick && captureSettings.endTick > targetTick)
            {
                Components::Playback::SetTickDelta(captureSettings.endTick - currentTick, true);
                return;
            }
            else if (captureSettings.startTick < currentTick && captureSettings.startTick > targetTick)
            {
                Components::Playback::SetTickDelta(captureSettings.startTick - currentTick, true);
                return;
            }
        }
        Components::Playback::SetTickDelta(value);
    }

    void HandlePlaybackInput()
    {
        if (Components::CaptureManager::Get().IsCapturing())
            return;
        
        static constexpr std::array TIMESCALE_STEPS = IWXMVM::Components::Playback::TIMESCALE_STEPS;

        if (Input::BindDown(Action::PlaybackToggle))
        {
            Components::Playback::TogglePaused();
        }

        if (Input::BindDown(Action::PlaybackFaster))
        {
            float& fTimescale = timescale.value().value->floating_point;

            if (const auto it = std::upper_bound(TIMESCALE_STEPS.begin(), TIMESCALE_STEPS.end(), fTimescale);
                it != TIMESCALE_STEPS.end())
                fTimescale = *it;
        }

        if (Input::BindDown(Action::PlaybackSlower))
        {
            float& fTimescale = timescale.value().value->floating_point;

            if (const auto it = std::upper_bound(TIMESCALE_STEPS.rbegin(), TIMESCALE_STEPS.rend(), fTimescale,
                                                 std::greater<float>());
                it != TIMESCALE_STEPS.rend())
                fTimescale = *it;
        }

        if (Input::BindDown(Action::PlaybackSkipForward))
        {
            SmartSetTickDelta(1000);
        }

        if (Input::BindDown(Action::PlaybackSkipBackward))
        {
            SmartSetTickDelta(-1000);
        }

        if (Input::BindDown(Action::TimeFrameMoveStart))
        {
            auto& captureManager = Components::CaptureManager::Get();
            auto& captureSettings = captureManager.GetCaptureSettings();
            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            captureSettings.startTick = demoInfo.currentTick;
        }

        if (Input::BindDown(Action::TimeFrameMoveEnd))
        {
            auto& captureManager = Components::CaptureManager::Get();
            auto& captureSettings = captureManager.GetCaptureSettings();
            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            captureSettings.endTick = demoInfo.currentTick;
        }
    }

    bool ControlBar::DrawDemoProgressBar(uint32_t* currentTick, uint32_t displayStartTick, uint32_t displayEndTick, uint32_t startTick, uint32_t endTick)
    {
        using namespace ImGui;
        auto label = "##demoProgressBar";

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const auto w = CalcItemWidth();
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(
            window->DC.CursorPos,
            window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));

        ItemSize(frame_bb, style.FramePadding.y);
        if (!ItemAdd(frame_bb, id, &frame_bb, 0))
            return false;

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        const bool clicked = hovered && IsMouseClicked(0, id);
        const bool make_active = (clicked || g.NavActivateId == id);
        if (make_active && clicked)
            SetKeyOwner(ImGuiKey_MouseLeft, id);

        if (make_active)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }

        if (displayStartTick > 0 || displayEndTick < endTick)
        {
            auto indicatorBarRect = ImRect(frame_bb.Min - ImVec2(0, 10), ImVec2(frame_bb.Max.x, frame_bb.Min.y - 2));

            window->DrawList->AddRectFilled(indicatorBarRect.Min, indicatorBarRect.Max,
                                            GetColorU32(ImGuiCol_FrameBg), 0, 0);

            auto startIndicatorOffset = (indicatorBarRect.Max.x - indicatorBarRect.Min.x) / endTick * displayStartTick;
            auto endIndicatorOffset = (indicatorBarRect.Max.x - indicatorBarRect.Min.x) / endTick * displayEndTick;
            window->DrawList->AddRectFilled(indicatorBarRect.Min + ImVec2(startIndicatorOffset, 0),
                                            indicatorBarRect.Min + ImVec2(endIndicatorOffset, 4),
                                            GetColorU32(ImGuiCol_Button), 0, 0);
        }

        // Draw frame
        const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive
                                            : hovered        ? ImGuiCol_FrameBgHovered
                                                             : ImGuiCol_FrameBg);
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        float halfNeedleWidth = 4.0f;

        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, ImGuiDataType_S32, currentTick, &displayStartTick,
                                                  &displayEndTick, "", ImGuiSliderFlags_NoInput, &grab_bb);
        
        auto t = static_cast<float>(*currentTick - displayStartTick) / static_cast<float>(displayEndTick - displayStartTick);
        grab_bb = ImRect(frame_bb.Min.x + t * w - halfNeedleWidth, frame_bb.Min.y,
                         frame_bb.Min.x + t * w + halfNeedleWidth, frame_bb.Max.y);
        if (value_changed)
            MarkItemEdited(id);

        // Render grab
        if (grab_bb.Max.x > grab_bb.Min.x && *currentTick >= displayStartTick && *currentTick <= displayEndTick)
        {
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);
        }

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, displayStartTick, displayEndTick, endTick);

        return value_changed;
    }

    void ControlBar::DrawCaptureRangeIndicator(int32_t displayStartTick, int32_t displayEndTick, float progressBarX,
                                               float progressBarWidth, ImVec2 pauseButtonSize,
                                               uint32_t* captureSettingsTargetTick, bool* draggingTimeframe)
    {
        const float tickRange = static_cast<float>(displayEndTick - displayStartTick);
        const ImVec2 localMouse = {
            ImGui::GetMousePos().x - GetPosition().x,
            ImGui::GetMousePos().y - GetPosition().y,
        };

        const auto textSize = ImGui::CalcTextSize(ICON_FA_CARET_UP);
        const auto percentage = static_cast<float>(*captureSettingsTargetTick - displayStartTick) / tickRange;
        const ImVec2 bbTopLeft = {
            progressBarX + percentage * progressBarWidth - textSize.x / 2,
            GetSize().y - pauseButtonSize.y / 2 - textSize.y * 0.2f,
        };
        const ImVec2 bbBottomRight = {
            bbTopLeft.x + textSize.x,
            bbTopLeft.y + textSize.y,
        };

        bool isHovered = localMouse.x >= bbTopLeft.x && localMouse.x <= bbBottomRight.x &&
                         localMouse.y >= bbTopLeft.y && localMouse.y <= bbBottomRight.y;

        if (isHovered)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (!*draggingTimeframe && isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            *draggingTimeframe = true;
        }
        else if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            *draggingTimeframe = false;
        }

        ImGui::SetCursorPos(bbTopLeft);
        if (*draggingTimeframe)
        {
            ImGui::SetCursorPosX(std::clamp(localMouse.x, progressBarX, progressBarX + progressBarWidth) -
                                 textSize.x / 2);
            *captureSettingsTargetTick = static_cast<std::uint32_t>(
                (std::clamp((localMouse.x - progressBarX), 0.0f, progressBarWidth) / progressBarWidth) * tickRange +
                displayStartTick);
        }

        ImGui::PushFont(UIManager::Get().GetBoldFont());
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ICON_FA_CARET_UP);
        ImGui::PopFont();
    }

    void ControlBar::DrawCaptureRangeIndicators(int32_t displayStartTick, int32_t displayEndTick, float progressBarX,
                                                float progressBarWidth, ImVec2 pauseButtonSize)
    {
        if (UIManager::Get().GetSelectedTab() != Tab::Record)
            return; 

        auto& captureManager = Components::CaptureManager::Get();
        auto& captureSettings = captureManager.GetCaptureSettings();

        // Left timeframe marker
        if (std::cmp_greater_equal(captureSettings.startTick, displayStartTick) && !captureManager.IsCapturing())
        {
            DrawCaptureRangeIndicator(displayStartTick, displayEndTick, progressBarX, progressBarWidth, pauseButtonSize,
                                      &captureSettings.startTick, &draggingStartTimeframe);
        }

        // Right timeframe marker
        if (std::cmp_less_equal(captureSettings.endTick, displayEndTick) && !captureManager.IsCapturing())
        {
            DrawCaptureRangeIndicator(displayStartTick, displayEndTick, progressBarX, progressBarWidth, pauseButtonSize,
                                      &captureSettings.endTick, &draggingEndTimeframe);
        }

        if (draggingStartTimeframe && draggingEndTimeframe)
        {
            draggingEndTimeframe = false;
        }

        if (captureSettings.startTick > captureSettings.endTick)
        {
            std::swap(captureSettings.startTick, captureSettings.endTick);
            std::swap(draggingStartTimeframe, draggingEndTimeframe);
        }
    }

    INCBIN_EXTERN(IMG_PLAY_BUTTON);
    INCBIN_EXTERN(IMG_PAUSE_BUTTON);

    void ControlBar::Render()
    {
        if (Mod::GetGameInterface()->GetGameState() == Types::GameState::MainMenu)
        {
            UIManager::Get().GetUIComponent(UI::Component::Readme)->Render();
        }

        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            return;

        if (!timescale.has_value())
        {
            timescale = Mod::GetGameInterface()->GetDvar("timescale");
            return;
        }

        HandlePlaybackInput();

        const auto padding = ImGui::GetStyle().WindowPadding;

        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y +
                           UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetFontSize() * 1.4f + padding.y * 3.0f + 4);

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        ImGui::BeginDisabled(Components::CaptureManager::Get().IsCapturing());
        
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        if (ImGui::Begin("Playback Controls", nullptr, flags))
        {
            const auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

            ImGui::SetCursorPosX(padding.x);
            ImGui::SetCursorPosY(GetSize().y / 2 - pauseButtonSize.y / 2);

            const auto image = UIImage::FromResource(
                Components::Playback::IsPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
                Components::Playback::IsPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);

            if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
            {
                Components::Playback::TogglePaused();
            }

            const auto playbackSpeedSliderWidth = GetSize().x / 8;

            ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
            ImGui::SetCursorPosX(padding.x + pauseButtonSize.x + ImGui::GetFontSize() * 0.8f);
            ImGui::SetCursorPosY(GetSize().y / 2 - pauseButtonSize.y / 2);
            ImGuiEx::TimescaleSlider("##1", &timescale.value().value->floating_point, 
                Components::Playback::TIMESCALE_STEPS.front(),
                Components::Playback::TIMESCALE_STEPS.back(),
                "%.3f",
                ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            const auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            if (demoInfo.currentTick < demoInfo.endTick)
            {
                const auto progressBarX = padding.x + pauseButtonSize.x + playbackSpeedSliderWidth + padding.x * 3;
                const auto progressBarWidth = GetSize().x - progressBarX - GetSize().x * 0.05f - padding.x;

                ImGui::SetCursorPosX(progressBarX + progressBarWidth + ImGui::GetFontSize() * 0.8f);
                ImGui::SetCursorPosY(GetSize().y / 2 - pauseButtonSize.y / 2);
                ImGui::Text("%s", std::format("{0}", demoInfo.currentTick).c_str());

                const auto keyframeEditor =
                    UIManager::Get().GetUIComponent<KeyframeEditor>(UI::Component::KeyframeEditor);
                const auto [displayStartTick, displayEndTick] = keyframeEditor->GetDisplayTickRange();

               DrawCaptureRangeIndicators(displayStartTick, displayEndTick, progressBarX, progressBarWidth, pauseButtonSize);

                ImGui::SetNextItemWidth(progressBarWidth);
                static uint32_t tickValue{};

                ImGui::SetCursorPosX(progressBarX);
                ImGui::SetCursorPosY(GetSize().y / 2 - pauseButtonSize.y / 2);
                const auto draggedProgressBar =
                    DrawDemoProgressBar(&tickValue, displayStartTick, displayEndTick, 0, demoInfo.endTick);

                if (draggedProgressBar && !Components::Rewinding::IsRewinding())
                {
                    Components::Playback::SetTickDelta(tickValue - demoInfo.currentTick);
                }
                else
                {
                    tickValue = demoInfo.currentTick;
                }
            }
        }

        ImGui::End();

        ImGui::EndDisabled();

        UIManager::Get().GetUIComponent(UI::Component::KeyframeEditor)->Render();
    }

    void ControlBar::Release()
    {
        UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
        UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
    }
}  // namespace IWXMVM::UI
