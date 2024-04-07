#include "StdInclude.hpp"
#include "ImGuiExtensions.hpp"

#include "Resources.hpp"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif


#include "Components/Playback.hpp"
#include "Utilities/MathUtils.hpp"
#include "Components/CameraManager.hpp"
#include "Components/CaptureManager.hpp"

namespace ImGuiEx
{
    void DrawLine3D(glm::vec3 from, glm::vec3 to, ImVec4 color, float thickness)
    {
        using namespace IWXMVM;

        const auto& camera = Components::CameraManager::Get().GetActiveCamera();
        auto screenPosition1 = MathUtils::WorldToScreenPoint(from, *camera);
        auto screenPosition2 = MathUtils::WorldToScreenPoint(to, *camera);

        if (screenPosition1.has_value() && screenPosition2.has_value())
        {
            ImGui::GetWindowDrawList()->AddLine(screenPosition1.value(), screenPosition2.value(),
                                                ImGui::ColorConvertFloat4ToU32(color), thickness);
        }
    }

    void DrawPoint3D(glm::vec3 point, ImVec4 color, ImVec2 size)
    {
        using namespace IWXMVM;

        const auto& camera = Components::CameraManager::Get().GetActiveCamera();
        auto screenPosition = MathUtils::WorldToScreenPoint(point, *camera);

        if (screenPosition.has_value())
        {
            ImGui::GetWindowDrawList()->AddRectFilled(screenPosition.value() - size, screenPosition.value() + size,
                                                      ImGui::ColorConvertFloat4ToU32(color));
        }
    }

    void DrawProgressLineAtTick(const ImRect rect, uint32_t tick, ImU32 color, float thickness,
                                uint32_t displayStartTick,
                                uint32_t displayEndTick)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        const auto barLength = rect.Max.x - rect.Min.x;

        const auto percentage =
            static_cast<float>(tick - displayStartTick) / static_cast<float>(displayEndTick - displayStartTick);
        const auto x = rect.Min.x + percentage * barLength - thickness / 2;

        if (x > rect.Max.x)
            return;

        window->DrawList->AddRectFilled(ImVec2(x, rect.Min.y), ImVec2(x + thickness, rect.Max.y), color);
    }

    constexpr std::size_t MARKER_DISTANCE = 5000;
    void DemoProgressBarLines(const ImRect rect, uint32_t currentTick, uint32_t displayStartTick,
                              uint32_t displayEndTick, std::uint32_t demoLength, std::optional<uint32_t> frozenTick)
    {
        using namespace ImGui;

        auto markerDistance = MARKER_DISTANCE * demoLength / 50000;
        for (uint32_t i = displayStartTick; i < currentTick; i++)
        {
            if (i % markerDistance == 0)
            {
                DrawProgressLineAtTick(rect, i, GetColorU32(ImGuiCol_Button), 2, displayStartTick, displayEndTick);
            }
        }

        auto captureSettings = IWXMVM::Components::CaptureManager::Get().GetCaptureSettings();
        DrawProgressLineAtTick(rect, captureSettings.startTick, GetColorU32(ImVec4(1, 0, 0, 1)), 2, displayStartTick,
                               displayEndTick);
        DrawProgressLineAtTick(rect, captureSettings.endTick, GetColorU32(ImVec4(1, 0, 0, 1)), 2, displayStartTick,
                               displayEndTick);

        if (frozenTick)
        {
            DrawProgressLineAtTick(rect, frozenTick.value(), GetColorU32(ImVec4(1, 1, 0, 1)), 2, displayStartTick,
                                   displayEndTick);
        }
    }

    bool TimescaleSliderInternal(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min,
                                 const void* p_max, const char* format, ImGuiSliderFlags flags, auto ClampValueFn)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = CalcItemWidth();

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos,
                              window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(
            frame_bb.Min,
            frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
            return false;

        // Default format string when passing NULL
        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
        if (!temp_input_is_active)
        {
            // Tabbing or CTRL-clicking on Slider turns it into an input box
            const bool input_requested_by_tabbing =
                temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = hovered && IsMouseClicked(0, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id);
            if (make_active && clicked)
                SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) ||
                    (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                    temp_input_is_active = true;

            if (make_active && !temp_input_is_active)
            {
                SetActiveID(id, window);
                SetFocusID(id, window);
                FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        if (temp_input_is_active)
        {
            // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
            const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
            return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL,
                                   is_clamp_input ? p_max : NULL);
        }

        // Draw frame
        const ImU32 frame_col = GetColorU32(g.ActiveId == id
                                                ? ImGuiCol_FrameBgActive
                                                : hovered
                                                ? ImGuiCol_FrameBgHovered
                                                : ImGuiCol_FrameBg);
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Slider behavior
        ImRect grab_bb;
        const bool value_changed =
            SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
        if (value_changed)
        {
            MarkItemEdited(id);

            ClampValueFn(static_cast<float*>(p_data));
        }

        // Render grab
        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end =
            value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
        if (g.LogEnabled)
            LogSetNextTextDecoration("{", "}");
        RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

        if (label_size.x > 0.0f)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        IMGUI_TEST_ENGINE_ITEM_INFO(
            id, label, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
        return value_changed;
    }

    bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format,
                         ImGuiSliderFlags flags)
    {
        auto ClampValue = [](void* p_data) {
            static constexpr double ROUNDING = 0.00000001;
            static constexpr std::array TIMESCALE_STEPS = IWXMVM::Components::Playback::TIMESCALE_STEPS;

            float* ptr = static_cast<float*>(p_data);
            const auto it = std::upper_bound(TIMESCALE_STEPS.begin(), TIMESCALE_STEPS.end(), *ptr - ROUNDING);

            *ptr = (it != TIMESCALE_STEPS.end()) ? *it : TIMESCALE_STEPS.back();
        };

        // TimescaleSliderInternal is a copy of SliderScalar with the intended behavior of being a slider with fixed
        // steps; the only change to SliderScalar is the above lambda and its execution, to clamp the slider value if it
        // has changed
        return ImGuiEx::TimescaleSliderInternal(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags,
                                                ClampValue);
    }
} // namespace ImGuiEx
