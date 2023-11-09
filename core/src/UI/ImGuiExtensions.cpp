#include "StdInclude.hpp"
#include "ImGuiExtensions.hpp"

#include "Resources.hpp"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui_internal.h"

#include "UIManager.hpp"
#include "Utilities/MathUtils.hpp"
#include "Components/KeyframeManager.hpp"
#include "Components/CameraManager.hpp"

namespace ImGuiEx
{
    void DrawLine3D(glm::vec3 from, glm::vec3 to, ImVec4 color, float thickness)
    {
        using namespace IWXMVM;

        const auto& camera = Components::CameraManager::Get().GetActiveCamera();

        auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
        auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y,
                                  gameView->GetPosition().x + gameView->GetSize().x,
                                  gameView->GetPosition().y + gameView->GetSize().y);
        auto screenPosition1 = MathUtils::WorldToScreenPoint(from, *camera, viewport);
        auto screenPosition2 = MathUtils::WorldToScreenPoint(to, *camera, viewport);

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

        auto& gameView = UI::UIManager::Get().GetUIComponent(UI::Component::GameView);
        auto viewport = glm::vec4(gameView->GetPosition().x, gameView->GetPosition().y,
                                  gameView->GetPosition().x + gameView->GetSize().x,
                                  gameView->GetPosition().y + gameView->GetSize().y);
        auto screenPosition = MathUtils::WorldToScreenPoint(point, *camera, viewport);

        if (screenPosition.has_value())
        {
            ImGui::GetWindowDrawList()->AddRectFilled(screenPosition.value() - size, screenPosition.value() + size,
                                                      ImGui::ColorConvertFloat4ToU32(color));
        }
    }

    constexpr std::size_t MARKER_DISTANCE = 5000;

    void DemoProgressBarLines(const ImRect rect, std::uint32_t currentTick, std::uint32_t endTick)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();

        const auto barLength = rect.Max.x - rect.Min.x;

        for (uint32_t i = MARKER_DISTANCE; i < currentTick; i += MARKER_DISTANCE)
        {
            const auto percentage = i / (float)endTick;
            const auto x = rect.Min.x + percentage * barLength;
            window->DrawList->AddRectFilled(ImVec2(x, rect.Min.y), ImVec2(x + 2, rect.Max.y),
                                            GetColorU32(ImGuiCol_Button));
        }
    }

    bool DemoProgressBar(int32_t* currentTick, uint32_t startTick, uint32_t endTick)
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
        const bool value_changed = SliderBehavior(frame_bb, id, ImGuiDataType_S32, currentTick, &startTick, &endTick,
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

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, endTick);

        return value_changed;
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
            static constexpr std::array TIMESCALE_STEPS{0.001, 0.002, 0.005, 0.01, 0.02, 0.025, 0.05, 0.075, 0.1,
                                                        0.125, 0.2, 0.25, 0.333, 0.5, 0.75, 1.0, 1.25, 1.5,
                                                        2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 500.0, 1000.0};

            float* ptr = static_cast<float*>(p_data);
            const auto it = std::upper_bound(TIMESCALE_STEPS.begin(), TIMESCALE_STEPS.end(), *ptr - ROUNDING);

            *ptr = (it != TIMESCALE_STEPS.end()) ? *it : TIMESCALE_STEPS.back();
        };

        // TimescaleSliderInternal is a copy of SliderScalar with the intended behavior of being a slider with fixed
        // steps the only change to SliderScalar is the above lambda and its execution, to clamp the slider value if it
        // has changed
        return ImGuiEx::TimescaleSliderInternal(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags,
                                                ClampValue);
    }

    ImVec2 GetPositionForKeyframe(
        const ImRect rect, const IWXMVM::Types::Keyframe& keyframe, const uint32_t endTick,
        const ImVec2 valueBoundaries = ImVec2(-10, 10),
        std::function<float(const IWXMVM::Types::Keyframe&)> GetKeyframeValue = [](const auto& keyframe) {
            return 0;
        })
    {
        const auto barHeight = rect.Max.y - rect.Min.y;
        const auto barLength = rect.Max.x - rect.Min.x;
        const auto percentage = static_cast<float>(keyframe.tick) / static_cast<float>(endTick);
        const auto x = rect.Min.x + percentage * barLength;
        const auto y = rect.Min.y + (1.0f - (GetKeyframeValue(keyframe) - valueBoundaries.x) /
                                     (valueBoundaries.y - valueBoundaries.x)) *
                       barHeight;
        return ImVec2(x, y);
    };

    std::tuple<uint32_t, IWXMVM::Types::KeyframeValue> GetKeyframeForPosition(
        const ImVec2 position, const ImRect rect, const uint32_t startTick, const uint32_t endTick,
        const ImVec2 valueBoundaries = ImVec2(-10, 10))
    {
        const auto barHeight = rect.Max.y - rect.Min.y;
        const auto barLength = rect.Max.x - rect.Min.x;
        const auto percentage = (position.x - rect.Min.x) / barLength;
        const auto tick = static_cast<uint32_t>(percentage * (endTick - startTick));
        const auto value =
            valueBoundaries.y - (position.y - rect.Min.y) / barHeight * (valueBoundaries.y - valueBoundaries.x);
        return std::make_tuple(tick, value);
    };

    void DrawKeyframeSliderInternal(const IWXMVM::Types::KeyframeableProperty& property, uint32_t* currentTick,
                                    uint32_t* startTick, uint32_t* endTick,
                                    std::vector<IWXMVM::Types::Keyframe>& keyframes)
    {
        using namespace ImGui;
        using namespace IWXMVM;

        auto flags = ImGuiSliderFlags_NoInput;
        auto data_type = ImGuiDataType_S32;

        ImGuiWindow* window = GetCurrentWindow();

        const char* label = property.name.data();

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

        ItemSize(total_bb, style.FramePadding.y);
        ItemAdd(total_bb, id, &frame_bb, 0);

        // Draw frame
        const ImU32 frame_col = GetColorU32(ImGuiCol_FrameBg);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Draw timeline indicator
        auto needleWidth = 4;

        auto t = static_cast<float>(*currentTick) / static_cast<float>(*endTick - *startTick);
        ImRect grab_bb = ImRect(frame_bb.Min.x + t * w - needleWidth / 2, frame_bb.Min.y,
                                frame_bb.Min.x + t * w + needleWidth / 2, frame_bb.Max.y);

        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        DemoProgressBarLines(frame_bb, *currentTick, *endTick);

        static Types::Keyframe* selectedKeyframe = nullptr;

        const auto textSize = CalcTextSize(ICON_FA_DIAMOND);
        for (auto& k : keyframes)
        {
            const auto position = GetPositionForKeyframe(frame_bb, k, *endTick);

            const auto barHeight = frame_bb.Max.y - frame_bb.Min.y;
            ImRect text_bb(ImVec2(position.x - textSize.x / 2, frame_bb.Min.y + (barHeight - textSize.y) / 2),
                           ImVec2(position.x + textSize.x / 2, frame_bb.Max.y - (barHeight - textSize.y) / 2));
            const bool hovered = ItemHoverable(text_bb, id, g.LastItemData.InFlags);

            if (hovered && IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedKeyframe = &k;
            }

            const ImU32 col = GetColorU32(hovered || selectedKeyframe == &k ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            window->DrawList->AddText(text_bb.Min, col, ICON_FA_DIAMOND);
        }

        if (selectedKeyframe != nullptr)
        {
            auto [tick, _] = GetKeyframeForPosition(GetMousePos(), frame_bb, *startTick, *endTick);
            selectedKeyframe->tick = tick;

            if (IsMouseReleased(ImGuiMouseButton_Left))
            {
                selectedKeyframe = nullptr;
            }
        }

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        if (hovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            auto [tick, _] = GetKeyframeForPosition(GetMousePos(), frame_bb, *startTick, *endTick);

            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) {
                    return k.tick == tick;
                }) ==
                keyframes.end())
            {
                // TODO: the value here should be the interpolated value between neighboring keyframes
                keyframes.push_back({property, tick, IWXMVM::Types::KeyframeValue(0)});
            }
        }
    }

    void DrawCurveEditorInternal(const IWXMVM::Types::KeyframeableProperty& property, uint32_t* currentTick,
                                 uint32_t* startTick, uint32_t* endTick, const float width,
                                 std::vector<IWXMVM::Types::Keyframe>& keyframes, int32_t keyframeValueIndex,
                                 std::function<float(const IWXMVM::Types::Keyframe&)> GetKeyframeValue,
                                 std::function<void(IWXMVM::Types::Keyframe&, float)> SetKeyframeValue)
    {
        using namespace ImGui;
        using namespace IWXMVM;

        ImGuiWindow* window = GetCurrentWindow();

        const char* label = property.name.data();

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const float curveEditorHeightMultiplier = 5.0f;
        const ImRect frame_bb(window->DC.CursorPos,
                              window->DC.CursorPos + ImVec2(width, label_size.y * curveEditorHeightMultiplier +
                                                                   style.FramePadding.y * 2.0f));
        const ImRect total_bb(
            frame_bb.Min,
            frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        ItemSize(total_bb, style.FramePadding.y);
        ItemAdd(total_bb, id, &frame_bb, 0);

        // Draw frame
        const ImU32 frame_col = GetColorU32(ImGuiCol_FrameBg);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Draw timeline indicator
        auto needleWidth = 4;

        auto t = static_cast<float>(*currentTick) / static_cast<float>(*endTick - *startTick);
        ImRect grab_bb = ImRect(frame_bb.Min.x + t * width - needleWidth / 2, frame_bb.Min.y,
                                frame_bb.Min.x + t * width + needleWidth / 2, frame_bb.Max.y);

        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        DemoProgressBarLines(frame_bb, *currentTick, *endTick);

        static Types::Keyframe* selectedKeyframe = nullptr;
        static int32_t selectedKeyframeValueIndex = -1;

        const auto textSize = CalcTextSize(ICON_FA_DIAMOND);
        const auto maxValue = std::max_element(
            keyframes.begin(), keyframes.end(),
            [GetKeyframeValue](const auto& a, const auto& b) {
                return GetKeyframeValue(a) < GetKeyframeValue(b);
            });
        const auto minValue = std::min_element(
            keyframes.begin(), keyframes.end(),
            [GetKeyframeValue](const auto& a, const auto& b) {
                return GetKeyframeValue(a) < GetKeyframeValue(b);
            });
        const ImVec2 valueBoundaries = ImVec2(minValue == keyframes.end() ? -10 : GetKeyframeValue(*minValue) - 10,
                                              maxValue == keyframes.end() ? 10 : GetKeyframeValue(*maxValue) + 10);

        for (auto& k : keyframes)
        {
            const auto position = GetPositionForKeyframe(frame_bb, k, *endTick, valueBoundaries, GetKeyframeValue);

            ImRect text_bb(ImVec2(position.x - textSize.x / 2, position.y - textSize.y / 2),
                           ImVec2(position.x + textSize.x / 2, position.y + textSize.y / 2));
            const bool hovered = ItemHoverable(text_bb, id, g.LastItemData.InFlags);

            if (hovered && IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedKeyframe = &k;
                selectedKeyframeValueIndex = keyframeValueIndex;
            }

            const ImU32 col = GetColorU32(hovered || selectedKeyframe == &k ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            window->DrawList->AddText(text_bb.Min, col, ICON_FA_DIAMOND);
            window->DrawList->AddText(ImVec2(text_bb.Max.x, text_bb.Min.y), GetColorU32(ImGuiCol_Text),
                                      std::format("{0:.0f}", GetKeyframeValue(k)).c_str());
        }

        if (!keyframes.empty())
        {
            const auto lowestTickKeyframe = std::min_element(keyframes.begin(), keyframes.end());
            const auto highestTickKeyframe = std::max_element(keyframes.begin(), keyframes.end());

            const auto EVALUATION_DISTANCE =
                glm::clamp(100 * (highestTickKeyframe->tick - lowestTickKeyframe->tick) / 5000, 50u, 1000u);

            auto previousKeyframe = *lowestTickKeyframe;
            for (auto tick = *startTick; tick <= *endTick; tick += EVALUATION_DISTANCE)
            {
                auto keyframe = Components::KeyframeManager::Get().Interpolate(property, tick);
                auto lastPosition =
                    GetPositionForKeyframe(frame_bb, previousKeyframe, *endTick, valueBoundaries, GetKeyframeValue);
                auto position = GetPositionForKeyframe(frame_bb, keyframe, *endTick, valueBoundaries, GetKeyframeValue);
                window->DrawList->AddLine(lastPosition, position, GetColorU32(ImGuiCol_Button));
                previousKeyframe = keyframe;
            }
        }

        if (selectedKeyframe != nullptr && selectedKeyframeValueIndex == keyframeValueIndex)
        {
            auto [tick, value] = GetKeyframeForPosition(GetMousePos(), frame_bb, *startTick, *endTick, valueBoundaries);

            selectedKeyframe->tick = tick;
            SetKeyframeValue(*selectedKeyframe, glm::fclamp(value.floatingPoint, -100'000.0f, 100'000.0f));

            if (IsMouseReleased(ImGuiMouseButton_Left))
            {
                selectedKeyframe = nullptr;
                selectedKeyframeValueIndex = -1;
            }
        }

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        if (hovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            auto [tick, value] = GetKeyframeForPosition(GetMousePos(), frame_bb, *startTick, *endTick, valueBoundaries);
            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) {
                    return k.tick == tick;
                }) ==
                keyframes.end())
            {
                keyframes.push_back({property, tick, value});
            }
        }
    }
} // namespace ImGuiEx
