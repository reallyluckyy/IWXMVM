#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "Components/CameraManager.hpp"
#include "Components/Playback.hpp"
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
            Mod::GetGameInterface()->SetTickDelta(500);
        }

        if (Input::BindDown(Action::PlaybackSkipBackward))
        {
            Mod::GetGameInterface()->SetTickDelta(-500);
        }
    }

    bool ControlBar::DrawDemoProgressBar(int32_t* currentTick, uint32_t displayStartTick, uint32_t displayEndTick, uint32_t startTick, uint32_t endTick)
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

        ImGui::BeginDisabled(Components::CaptureManager::Get().IsCapturing());
        
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Playback Controls", nullptr, flags))
        {

            ImGui::SetCursorPosX(padding.x);

            auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

            auto image = UIImage::FromResource(
                Components::Playback::IsPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
                Components::Playback::IsPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);

            if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
                Components::Playback::TogglePaused();

            const auto playbackSpeedSliderWidth = GetSize().x / 8;

            ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
            ImGui::SameLine();
            ImGuiEx::TimescaleSlider("##1", &timescale.value().value->floating_point, 
                Components::Playback::TIMESCALE_STEPS.front(),
                Components::Playback::TIMESCALE_STEPS.back(),
                "%.3f",
                ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto progressBarX = padding.x + pauseButtonSize.x + playbackSpeedSliderWidth + padding.x * 3;
            auto progressBarWidth = GetSize().x - progressBarX - GetSize().x * 0.05f - padding.x;

            ImGui::SameLine(progressBarX);
            ImGui::SetNextItemWidth(progressBarWidth);
            static std::int32_t tickValue{};

            auto keyframeEditor = UIManager::Get().GetUIComponent<KeyframeEditor>(UI::Component::KeyframeEditor);
            auto [displayStartTick, displayEndTick] = keyframeEditor->GetDisplayTickRange();
            if (!DrawDemoProgressBar(&tickValue, displayStartTick, displayEndTick, 0, demoInfo.endTick))
                tickValue = demoInfo.currentTick;
            else
                Mod::GetGameInterface()->SetTickDelta(tickValue - demoInfo.currentTick);

            ImGui::SameLine();
            ImGui::Text("%s", std::format("{0}", demoInfo.currentTick).c_str());

            ImGui::End();
        }

        ImGui::EndDisabled();

        UIManager::Get().GetUIComponent(UI::Component::KeyframeEditor)->Render();
    }

    void ControlBar::Release()
    {
        UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
        UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
    }
}  // namespace IWXMVM::UI
