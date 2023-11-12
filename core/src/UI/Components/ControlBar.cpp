#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "Components/CameraManager.hpp"
#include "UI/ImGuiExtensions.hpp"
#include "UI/UIImage.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Input.hpp"
#include "Events.hpp"

namespace IWXMVM::UI
{
    float playbackSpeed;
    int32_t displayStartTick, displayEndTick;

    std::optional<Types::Dvar> timescale;

    void ControlBar::Initialize()
    {
        playbackSpeed = 1.0f;

        Events::RegisterListener(EventType::OnDemoLoad, []() {
            displayStartTick = 0;
            displayEndTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        });
    }

    void HandlePlaybackInput()
    {
        // this should probably go somewhere else eventually

        if (Input::BindDown(Action::PlaybackToggle))
        {
            Mod::GetGameInterface()->ToggleDemoPlaybackState();
        }

        if (Input::BindDown(Action::PlaybackFaster))
        {
            timescale.value().value->floating_point *= 2.0f;
        }

        if (Input::BindDown(Action::PlaybackSlower))
        {
            timescale.value().value->floating_point /= 2.0f;
        }

        if (Input::BindDown(Action::PlaybackSkipForward))
        {
            Mod::GetGameInterface()->SetTickDelta(500);
        }

        if (Input::BindDown(Action::PlaybackSkipBackward))
        {
            Mod::GetGameInterface()->SetTickDelta(-500);
        }
    }

    void HandleTimelineZoomInteractions()
    {
        const int32_t ZOOM_AMOUNT = Mod::GetGameInterface()->GetDemoInfo().endTick / 200;
        if (Input::GetScrollDelta() > 0)
        {
            displayStartTick = glm::clamp(displayStartTick + ZOOM_AMOUNT, 0, displayEndTick - ZOOM_AMOUNT);
            displayEndTick = glm::clamp(displayEndTick - ZOOM_AMOUNT, displayStartTick + ZOOM_AMOUNT,
                                        (int32_t)Mod::GetGameInterface()->GetDemoInfo().endTick);
        }
        else if (Input::GetScrollDelta() < 0)
        {
            displayStartTick = glm::clamp(displayStartTick - ZOOM_AMOUNT, 0, displayEndTick - ZOOM_AMOUNT);
            displayEndTick = glm::clamp(displayEndTick + ZOOM_AMOUNT, displayStartTick + ZOOM_AMOUNT,
                                        (int32_t)Mod::GetGameInterface()->GetDemoInfo().endTick);
        }
    }

    bool DrawDemoProgressBar(int32_t* currentTick, uint32_t displayStartTick, uint32_t displayEndTick, uint32_t startTick, uint32_t endTick)
    {
        using namespace ImGui;
        auto label = "##demoProgressBar";
        auto format = "%d";

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(
            window->DC.CursorPos,
            window->DC.CursorPos + ImVec2(CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(frame_bb.Min, frame_bb.Max);

        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb, 0))
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

        if (hovered)
        {
            HandleTimelineZoomInteractions();
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

        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, ImGuiDataType_S32, currentTick, &displayStartTick,
                                                  &displayEndTick,
                                                  format, ImGuiSliderFlags_NoInput, &grab_bb);
        if (value_changed)
            MarkItemEdited(id);

        // Render grab
        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf),
                                                                     ImGuiDataType_S32, currentTick, format);
        RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, displayEndTick);

        return value_changed;
    }

    INCBIN_EXTERN(IMG_PLAY_BUTTON);
    INCBIN_EXTERN(IMG_PAUSE_BUTTON);

    void ControlBar::Render()
    {
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
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetFontSize() * 1.4f + padding.y * 2 + 4);

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Playback Controls", nullptr, flags))
        {
            ImGui::SetCursorPosX(padding.x);

            auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

            auto image = UIImage::FromResource(
                Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
                Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);

            if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
                Mod::GetGameInterface()->ToggleDemoPlaybackState();

            const auto playbackSpeedSliderWidth = GetSize().x / 8;

            ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
            ImGui::SameLine();
            ImGuiEx::TimescaleSlider("##1", &timescale.value().value->floating_point, 0.001f, 1000.0f, "%.3f",
                                     ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto progressBarX = padding.x + pauseButtonSize.x + playbackSpeedSliderWidth + padding.x * 3;
            auto progressBarWidth = GetSize().x - progressBarX - GetSize().x * 0.05f - padding.x;

            ImGui::SameLine(progressBarX);
            ImGui::SetNextItemWidth(progressBarWidth);
            static std::int32_t tickValue{};

            if (!DrawDemoProgressBar(&tickValue, displayStartTick, displayEndTick, 0, demoInfo.endTick))
                tickValue = demoInfo.currentTick;
            else
                Mod::GetGameInterface()->SetTickDelta(tickValue - demoInfo.currentTick);

            ImGui::SameLine();
            ImGui::Text("%s", std::format("{0}", demoInfo.currentTick).c_str());

            ImGui::End();
        }

        UIManager::Get().GetUIComponent(UI::Component::KeyframeEditor)->Render();
    }

    void ControlBar::Release()
    {
        UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
        UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
    }
}  // namespace IWXMVM::UI
