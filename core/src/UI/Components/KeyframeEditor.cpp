
#include "StdInclude.hpp"
#include "KeyframeEditor.hpp"

#include <IconsFontAwesome6.h>
#include <imgui_internal.h>

#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/ImGuiExtensions.hpp"
#include "Mod.hpp"
#include "Input.hpp"
#include "Components/KeyframeManager.hpp"
#include "Components/KeyframeSerializer.hpp"
#include "Components/Rewinding.hpp"
#include "Components/Playback.hpp"
#include "Events.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{
    std::map<Types::KeyframeablePropertyType, std::vector<ImVec2>> verticalZoomRanges;

    std::tuple<int32_t, int32_t> KeyframeEditor::GetDisplayTickRange() const
    {
        return {displayStartTick, displayEndTick};
    }

    ImVec2 GetPositionForKeyframe(const ImRect rect, const Types::Keyframe& keyframe, const uint32_t startTick,
                                  const uint32_t endTick, const ImVec2 valueBoundaries = ImVec2(-1, 1),
                                  int32_t valueIndex = 0)
    {
        const auto barHeight = rect.Max.y - rect.Min.y;
        const auto barLength = rect.Max.x - rect.Min.x;
        const auto tickPercentage =
            static_cast<float>(keyframe.tick - startTick) / static_cast<float>(endTick - startTick);
        const auto x = rect.Min.x + tickPercentage * barLength;
        const auto valuePercentage =
            1 - (keyframe.value.GetByIndex(valueIndex) - valueBoundaries.x) / (valueBoundaries.y - valueBoundaries.x);
        const auto y = rect.Min.y + valuePercentage * barHeight;
        return ImVec2(x, y);
    };

    uint32_t GetTickForPosition(const float x, const ImRect rect, const uint32_t startTick, const uint32_t endTick)
    {
        const auto barLength = rect.Max.x - rect.Min.x;
        const auto tickPercentage = (x - rect.Min.x) / barLength;
        return static_cast<uint32_t>(startTick + tickPercentage * (endTick - startTick));
    }

    std::tuple<uint32_t, Types::KeyframeValue> GetKeyframeForPosition(const ImVec2 position, const ImRect rect,
                                                                      const uint32_t startTick, const uint32_t endTick,
                                                                      const ImVec2 valueBoundaries = ImVec2(-1, 1))
    {
        const auto tick = GetTickForPosition(position.x, rect, startTick, endTick);
        const auto barHeight = rect.Max.y - rect.Min.y;
        const auto valuePercentage = (position.y - rect.Min.y) / barHeight;
        const auto value = valueBoundaries.x + (1 - valuePercentage) * (valueBoundaries.y - valueBoundaries.x);
        return std::make_tuple(tick, value);
    };

    void HandleVerticalPan(const ImVec2 rectMin, const ImVec2 rectMax, const Types::KeyframeableProperty& property,
        std::optional<int32_t> valueIndex)
    {
        static int32_t heldValueIndex = -1;

        if (valueIndex.has_value() && Input::KeyDown(ImGuiKey_MouseLeft))
        {
            heldValueIndex = valueIndex.value();
        }

        if (valueIndex.has_value() && Input::KeyUp(ImGuiKey_MouseLeft))
        {
            heldValueIndex = -1;
        }

        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            if (valueIndex.has_value() && heldValueIndex == valueIndex.value())
            {
                auto currentRange = verticalZoomRanges[property.type][valueIndex.value()];
                auto scaledDelta = delta / (rectMax.y - rectMin.y);
                scaledDelta *= currentRange.y - currentRange.x;
                verticalZoomRanges[property.type][valueIndex.value()].x += scaledDelta.y;
                verticalZoomRanges[property.type][valueIndex.value()].y += scaledDelta.y;
            }

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
    }

    void KeyframeEditor::HandleTimelineZoomInteractions(const ImVec2 rectMin, const ImVec2 rectMax, const Types::KeyframeableProperty& property, std::optional<int32_t> valueIndex)
    {
        constexpr auto ZOOM_MULTIPLIER = 2000;
        constexpr auto MOVE_MULTIPLIER = 1;

        constexpr int32_t MINIMUM_ZOOM = 500;

        const auto minTick = 0;
        const auto maxTick = (int32_t)Mod::GetGameInterface()->GetDemoInfo().endTick;

        auto scrollDelta = Input::GetScrollDelta() * Input::GetDeltaTime();

        // center zoom section around mouse cursor if zooming in
        const auto barLength = rectMax.x - rectMin.x;
        const auto mousePercentage = (ImGui::GetMousePos().x - rectMin.x) / barLength;

        scrollDelta = scrollDelta * 100 * ZOOM_MULTIPLIER * maxTick / 50000;
        displayStartTick += static_cast<uint32_t>(scrollDelta * mousePercentage);
        displayStartTick = glm::clamp(displayStartTick, minTick, displayEndTick - MINIMUM_ZOOM);
        displayEndTick -= static_cast<uint32_t>(scrollDelta * (1.0f - mousePercentage));
        displayEndTick = glm::clamp(displayEndTick, displayStartTick + MINIMUM_ZOOM, maxTick);

        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.1f);

        if (glm::abs(delta.x) > glm::abs(delta.y) * 1.25f)
        {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                auto zoomWindowSizeBefore = displayEndTick - displayStartTick;

                auto scaledDelta = delta / (rectMax.x - rectMin.x);
                scaledDelta *= (displayEndTick - displayStartTick);

                displayStartTick = glm::max(displayStartTick - (int32_t)(scaledDelta.x * MOVE_MULTIPLIER), minTick);
                displayEndTick = glm::min(displayEndTick - (int32_t)(scaledDelta.x * MOVE_MULTIPLIER), maxTick);

                auto zoomWindowSizeAfter = displayEndTick - displayStartTick;

                if (zoomWindowSizeAfter != zoomWindowSizeBefore)
                {
                    if (displayStartTick == minTick)
                        displayEndTick = displayStartTick + zoomWindowSizeBefore;
                    else
                        displayStartTick = displayEndTick - zoomWindowSizeBefore;
                }

                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }
        }
        else
        {
            HandleVerticalPan(rectMin, rectMax, property, valueIndex); 
        }

    }

    std::optional<int32_t> selectedKeyframeId = std::nullopt;
    std::optional<int32_t> selectedKeyframeValueIndex = std::nullopt;

    void KeyframeEditor::DrawKeyframeSliderInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                                    uint32_t displayStartTick, uint32_t displayEndTick,
                                                    std::vector<Types::Keyframe>& keyframes, uint32_t demoLength)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();

        const char* label = property.name.data();

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = CalcItemWidth();

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos,
                              window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));

        ItemSize(frame_bb, style.FramePadding.y);
        ItemAdd(frame_bb, id, &frame_bb, 0);

        // Draw frame
        const ImU32 frame_col = GetColorU32(ImGuiCol_FrameBg);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Draw timeline indicator
        float halfNeedleWidth = 2;

        auto t =
            static_cast<float>(*currentTick - displayStartTick) / static_cast<float>(displayEndTick - displayStartTick);
        ImRect grab_bb = ImRect(frame_bb.Min.x + t * w - halfNeedleWidth, frame_bb.Min.y,
                                frame_bb.Min.x + t * w + halfNeedleWidth, frame_bb.Max.y);

        if (grab_bb.Max.x < frame_bb.Max.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, displayStartTick, displayEndTick, demoLength);

        bool isAnyKeyframeHovered = false;

        const auto textSize = CalcTextSize(ICON_FA_DIAMOND);
        for (auto it = keyframes.begin(); it != keyframes.end();)
        {
            auto& k = *it;

            const auto positionX = GetPositionForKeyframe(frame_bb, k, displayStartTick, displayEndTick).x;

            const auto barHeight = frame_bb.Max.y - frame_bb.Min.y;
            ImRect text_bb(ImVec2(positionX - textSize.x / 2, frame_bb.Min.y + (barHeight - textSize.y) / 2),
                           ImVec2(positionX + textSize.x / 2, frame_bb.Max.y - (barHeight - textSize.y) / 2));
            const bool hovered = ItemHoverable(text_bb, id, g.LastItemData.InFlags);

            isAnyKeyframeHovered |= hovered;

            if (hovered && IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedKeyframeId = k.id;
                selectedKeyframeValueIndex = std::nullopt;
            }

            const ImU32 col =
                GetColorU32(hovered || selectedKeyframeId == k.id ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            window->DrawList->AddText(text_bb.Min, col, ICON_FA_DIAMOND);

            if (hovered && IsMouseClicked(ImGuiMouseButton_Right))
            {
                keyframes.erase(it);
                break;
            }

            if (selectedKeyframeId == k.id)
            {
                auto [tick, _] = GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick);
                k.tick = tick;
                Components::KeyframeManager::Get().SortAndSaveKeyframes(keyframes);

                if (IsMouseReleased(ImGuiMouseButton_Left))
                {
                    selectedKeyframeId = std::nullopt;
                    selectedKeyframeValueIndex = std::nullopt;
                }
            }

            ++it;
        }

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        if (hovered && !isAnyKeyframeHovered && !selectedKeyframeId.has_value())
        {
            HandleTimelineZoomInteractions(frame_bb.Min, frame_bb.Max, property, std::nullopt);
        }

        if (hovered && !isAnyKeyframeHovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            auto [tick, _] = GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick);

            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) { return k.tick == tick; }) ==
                keyframes.end())
            {
                keyframes.emplace_back(property, tick, Components::KeyframeManager::Get().Interpolate(property, tick));
                Components::KeyframeManager::Get().SortAndSaveKeyframes(keyframes);
            }
        }
    }


    ImVec2& GetValueDisplayRange(const Types::KeyframeableProperty& property,
                                                  const std::vector<Types::Keyframe>& keyframes,
                                                  int32_t keyframeValueIndex)
    {
        ImVec2 valueBoundaries = ImVec2(-1.0, 1.0);
        auto& propertyRanges = verticalZoomRanges.at(property.type);
        return propertyRanges.at(keyframeValueIndex);

        return valueBoundaries;
    }

    bool PositionInBoundingBox(ImVec2 position, ImRect rect)
    {
        return position.x > rect.Min.x && position.x < rect.Max.x && position.y > rect.Min.y &&
               position.y < rect.Max.y;
    }

    void DrawVerticalZoomButtons(const Types::KeyframeableProperty& property, int32_t valueIndex)
    {
        auto buttonSize = ImVec2(1, 1) * ImGui::GetFontSize() * 1.5f;
        auto cursorPosition = ImGui::GetCursorPos();

        auto& currentRange = verticalZoomRanges[property.type][valueIndex];

        auto halfMagnitude = (currentRange.y - currentRange.x) * 0.5f;
        auto center = halfMagnitude + currentRange.x;

        ImGui::PushID(std::format("{}{}zoomButtons", property.name, valueIndex).c_str());

        if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS_PLUS "##verticalZoomInButton", buttonSize))
        {
            currentRange.x = center - halfMagnitude * 0.75f;
            currentRange.y = center + halfMagnitude * 0.75f;
        }

        ImGui::SetCursorPos(cursorPosition + ImVec2(0, buttonSize.y + ImGui::GetStyle().ItemSpacing.y));
        if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN "##verticalZoomResetButton", buttonSize))
        {
            const auto& keyframes = Components::KeyframeManager::Get().GetKeyframes(property);
            if (!keyframes.empty())
            {
                const auto minValueKeyframe =
                    std::min_element(keyframes.begin(), keyframes.end(), [valueIndex](const auto& a, const auto& b) {
                        return a.value.GetByIndex(valueIndex) < b.value.GetByIndex(valueIndex);
                    });

                const auto maxValueKeyframe =
                    std::max_element(keyframes.begin(), keyframes.end(), [valueIndex](const auto& a, const auto& b) {
                        return a.value.GetByIndex(valueIndex) < b.value.GetByIndex(valueIndex);
                    });

                auto minValue = minValueKeyframe->value.GetByIndex(valueIndex);
                auto maxValue = maxValueKeyframe->value.GetByIndex(valueIndex);

                currentRange.x = (minValue < 0 ? minValue * 1.15f : minValue / 1.15f) - 0.1f;
                currentRange.y = (maxValue < 0 ? maxValue / 1.15f : maxValue * 1.15f) + 0.1f;
            }
        }

        ImGui::SetCursorPos(cursorPosition + ImVec2(0, buttonSize.y + ImGui::GetStyle().ItemSpacing.y) * 2);
        if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS_MINUS "##verticalZoomOutButton", buttonSize))
        {
            currentRange.x = center - halfMagnitude * 1.5f;
            currentRange.y = center + halfMagnitude * 1.5f;
        }

        ImGui::PopID();
    }

    const float CURVE_EDITOR_LANE_HEIGHT = 200.0f;
    constexpr float KEYFRAME_MAX_VALUE = 10'000.0f;

    void KeyframeEditor::DrawCurveEditorInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                                 uint32_t displayStartTick, uint32_t displayEndTick, const float width,
                                                 std::vector<Types::Keyframe>& keyframes, int32_t keyframeValueIndex,
                                                 uint32_t demoLength)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();

        const char* label = property.name.data();

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, CURVE_EDITOR_LANE_HEIGHT));
        const ImRect total_bb(
            frame_bb.Min,
            frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        ItemSize(total_bb, style.FramePadding.y);
        ItemAdd(total_bb, id, &frame_bb, 0);

        // Draw frame
        const ImU32 frame_col = GetColorU32(ImGuiCol_FrameBg);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Draw timeline indicator
        float halfNeedleWidth = 2;

        auto t =
            static_cast<float>(*currentTick - displayStartTick) / static_cast<float>(displayEndTick - displayStartTick);
        ImRect grab_bb = ImRect(frame_bb.Min.x + t * width - halfNeedleWidth, frame_bb.Min.y,
                                frame_bb.Min.x + t * width + halfNeedleWidth, frame_bb.Max.y);

        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, displayStartTick, displayEndTick, demoLength);

        bool isAnyKeyframeHovered = false;

        const auto textSize = CalcTextSize(ICON_FA_DIAMOND);

        auto valueBoundaries = GetValueDisplayRange(property, keyframes, keyframeValueIndex);

        if (!keyframes.empty())
        {
            const auto lowestTickKeyframe = std::min_element(keyframes.begin(), keyframes.end());
            const auto highestTickKeyframe = std::max_element(keyframes.begin(), keyframes.end());

            const auto EVALUATION_DISTANCE =
                glm::clamp(100 * (highestTickKeyframe->tick - lowestTickKeyframe->tick) / 5000, 50u, 1000u);

            std::vector<ImVec2> polylinePoints;

            for (auto tick = displayStartTick; tick <= displayEndTick; tick += EVALUATION_DISTANCE)
            {
                auto value = Components::KeyframeManager::Get().Interpolate(property, keyframes, tick);
                auto position =
                    GetPositionForKeyframe(frame_bb, Types::Keyframe(property, tick, value), displayStartTick,
                                           displayEndTick, valueBoundaries, keyframeValueIndex);

                if (!PositionInBoundingBox(position, frame_bb))
                {
                    continue;
                }

                polylinePoints.push_back(position);
            }

            window->DrawList->AddPolyline(polylinePoints.data(), polylinePoints.size(), GetColorU32(ImGuiCol_Button), 0,
                                          2.0f);
        }

        for (auto it = keyframes.begin(); it != keyframes.end();)
        {
            auto& k = *it;

            const auto position = GetPositionForKeyframe(frame_bb, k, displayStartTick, displayEndTick, valueBoundaries,
                                                         keyframeValueIndex);

            if (!PositionInBoundingBox(position, frame_bb))
            {
                ++it;
                continue;
            }

            ImRect text_bb(ImVec2(position.x - textSize.x / 2, position.y - textSize.y / 2),
                           ImVec2(position.x + textSize.x / 2, position.y + textSize.y / 2));
            const bool hovered = ItemHoverable(text_bb, id, g.LastItemData.InFlags);

            isAnyKeyframeHovered |= hovered;

            if (hovered && IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedKeyframeId = k.id;
                selectedKeyframeValueIndex = keyframeValueIndex;
            }

            auto currentValue = k.value.GetByIndex(keyframeValueIndex);
            auto keyframeValueLabel =
                currentValue < 20 ? std::format("{0:.2f}", currentValue) : std::format("{0:.0f}", currentValue);

            const ImU32 col =
                GetColorU32(hovered || selectedKeyframeId == k.id ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            window->DrawList->AddText(text_bb.Min, col, ICON_FA_DIAMOND);
            window->DrawList->AddText(ImVec2(text_bb.Max.x + 2, text_bb.Min.y), GetColorU32(ImGuiCol_Text),
                                      keyframeValueLabel.c_str());

            if (selectedKeyframeId == k.id && selectedKeyframeValueIndex == keyframeValueIndex)
            {
                auto [tick, value] =
                    GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick, valueBoundaries);

                k.tick = tick;

                value.floatingPoint = glm::fclamp(value.floatingPoint, valueBoundaries.x, valueBoundaries.y);
                k.value.SetByIndex(keyframeValueIndex,
                                   glm::fclamp(value.floatingPoint, -KEYFRAME_MAX_VALUE, KEYFRAME_MAX_VALUE));
                Components::KeyframeManager::Get().SortAndSaveKeyframes(keyframes);

                if (IsMouseReleased(ImGuiMouseButton_Left))
                {
                    selectedKeyframeId = std::nullopt;
                    selectedKeyframeValueIndex = std::nullopt;
                }
            }

            const auto POPUP_LABEL = std::format("##editKeyframeValue{0}.{1}", k.id, keyframeValueIndex);
            if (hovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                ImGui::OpenPopup(POPUP_LABEL.c_str());
            }

            if (ImGui::BeginPopup(POPUP_LABEL.c_str()))
            {
                auto currentValue = k.value.GetByIndex(keyframeValueIndex);

                ImGui::Text("Edit keyframe value:");

                ImGui::Separator();

                ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6.0f);

                ImGui::SetKeyboardFocusHere();
                if (ImGui::DragFloat("##valueInput", &currentValue, 0.1f, -KEYFRAME_MAX_VALUE, KEYFRAME_MAX_VALUE,
                                     "%.2f"))
                {
                    k.value.SetByIndex(keyframeValueIndex, currentValue);
                    Components::KeyframeManager::Get().SortAndSaveKeyframes(keyframes);
                }

                if (ImGui::IsKeyPressed(ImGuiKey_Enter))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            if (hovered && IsMouseClicked(ImGuiMouseButton_Right))
            {
                keyframes.erase(it);
                break;
            }

            ++it;
        }

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        if (hovered && !isAnyKeyframeHovered && !selectedKeyframeId.has_value())
        {
            HandleTimelineZoomInteractions(frame_bb.Min, frame_bb.Max, property, keyframeValueIndex);
        }

        if (hovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            auto [tick, valueAtMouse] =
                GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick, valueBoundaries);
            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) { return k.tick == tick; }) ==
                keyframes.end())
            {
                auto value = Components::KeyframeManager::Get().Interpolate(property, tick);
                value.SetByIndex(keyframeValueIndex, valueAtMouse.floatingPoint);
                keyframes.emplace_back(property, tick, value);
                Components::KeyframeManager::Get().SortAndSaveKeyframes(keyframes);
            }
        }
    }

    void KeyframeEditor::Initialize()
    {
        Events::RegisterListener(EventType::PostDemoLoad, [this]() {
            displayStartTick = 0;
            displayEndTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        });

        for (const auto& pair : Components::KeyframeManager::Get().GetKeyframes())
        {
            propertyVisible[pair.first] = false;

            verticalZoomRanges[pair.first.type] = std::vector<ImVec2>(pair.first.GetValueCount());
            for (int i = 0; i < pair.first.GetValueCount(); i++)
                verticalZoomRanges[pair.first.type][i] =
                    ImVec2(std::get<0>(pair.first.defaultValueRange), std::get<1>(pair.first.defaultValueRange));
        }
    }

    void KeyframeEditor::DrawKeyframeSlider(const Types::KeyframeableProperty& property)
    {
        auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
        auto currentTick = demoInfo.currentTick;
        auto [displayStartTick, displayEndTick] = GetDisplayTickRange();

        DrawKeyframeSliderInternal(property, &currentTick, displayStartTick, displayEndTick,
                                   Components::KeyframeManager::Get().GetKeyframes(property), demoInfo.endTick);
    }

    void KeyframeEditor::DrawCurveEditor(const Types::KeyframeableProperty& property, const auto width)
    {
        auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
        auto currentTick = demoInfo.currentTick;
        auto [displayStartTick, displayEndTick] = GetDisplayTickRange();

        // TODO: Probably disable for CameraData, as I'm not sure if this is helpful to the user at all

        // if (property.valueType == Types::KeyframeValueType::CameraData)
        //    return;

        for (int i = 0; i < property.GetValueCount(); i++)
        {
            DrawCurveEditorInternal(property, &currentTick, displayStartTick, displayEndTick, width,
                                    Components::KeyframeManager::Get().GetKeyframes(property), i, demoInfo.endTick);
        }
    }

    void DrawKeyframeValueInput(const char* label, float dvalue, const Types::KeyframeableProperty& property, int index)
    {
        std::vector<Types::Keyframe>& keyframes = Components::KeyframeManager::Get().GetKeyframes(property);
        static float v = 0;

        v = dvalue;
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6.0f);
        if (ImGui::DragFloat(label, &v, 0.1f, -KEYFRAME_MAX_VALUE, KEYFRAME_MAX_VALUE, "%.2f"))
        {
            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto currentTick = demoInfo.currentTick;
            auto [tick, val] = std::make_tuple(currentTick, v);
            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) { return k.tick == tick; }) ==
                keyframes.end())
            {
                auto value = Components::KeyframeManager::Get().Interpolate(property, tick);
                keyframes.emplace_back(property, tick, value);
            }
            else
            {
                auto keyframe = std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) { return k.tick == tick; });
                keyframe->value.SetByIndex(index, v);
            }
            Components::KeyframeManager::Get().SortAndSaveKeyframes(keyframes);
        }
    }

    void DrawLeftArrow(const char* label, const Types::KeyframeableProperty& property)
    {
        static auto lastKeyFrameTime = 0;
        if (lastKeyFrameTime != 0 && !Components::Rewinding::IsRewinding())
        {
            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto currentTick = demoInfo.currentTick;
            Components::Playback::SkipForward(lastKeyFrameTime - demoInfo.currentTick);

            lastKeyFrameTime = 0;
        }

        std::vector<Types::Keyframe>& keyframes = Components::KeyframeManager::Get().GetKeyframes(property);
        ImGui::SameLine();
        if (ImGui::ArrowButton(label, 0))
        {
            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto currentTick = demoInfo.currentTick;
            auto previousKeyframe = *keyframes.begin();
            for (const auto& keyframe : keyframes)
            {
                if (keyframe.tick < currentTick)
                    previousKeyframe = keyframe;
            }
            if (previousKeyframe.tick < currentTick)
            {
                lastKeyFrameTime = previousKeyframe.tick;
                Components::Rewinding::RewindBy(previousKeyframe.tick - demoInfo.currentTick - 50);
            }
        }
    }

     void DrawRightArrow(const char* label, const Types::KeyframeableProperty& property)
    {
        std::vector<Types::Keyframe>& keyframes = Components::KeyframeManager::Get().GetKeyframes(property);
        ImGui::SameLine();
        if (ImGui::ArrowButton(label, 1))
        {
            auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
            auto currentTick = demoInfo.currentTick;
            auto nextKeyframe = *keyframes.rbegin();
            for (std::vector<Types::Keyframe>::reverse_iterator keyframe = keyframes.rbegin(); keyframe != keyframes.rend(); ++keyframe)
            {
                if (keyframe->tick > currentTick)
                    nextKeyframe = *keyframe;
            }
            if (nextKeyframe.tick > currentTick)
            {
                Components::Playback::SkipForward(nextKeyframe.tick - demoInfo.currentTick);
            }
        }
    }

    constexpr auto CLEAR_KEYFRAMES_POPUP_LABEL = "Are you sure?##clearKeyframes";
    void KeyframeEditor::DrawMiscButtons(ImVec2 padding, bool hasKeyframes)
    {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFontSize() * 2 - padding.y);
        if (hasKeyframes)
        {
            if (ImGui::Button(ICON_FA_FILE_ARROW_DOWN " Export", ImVec2(GetSize().x / 20, 0)))
            {
                auto path = PathUtils::OpenFileDialog(true, OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
                                                      "Keyframes (*.json)\0*.json\0", "json");
                if (path.has_value())
                {
                    Components::KeyframeSerializer::Write(path.value());
                    LOG_INFO("Wrote keyframes to {}", path.value().string());
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_TRASH_CAN " Clear", ImVec2(GetSize().x / 20, 0)))
            {
                ImGui::OpenPopup(CLEAR_KEYFRAMES_POPUP_LABEL);
            }
        }
        else
        {
            if (ImGui::Button(ICON_FA_FILE_IMPORT " Import", ImVec2(GetSize().x / 20, 0)))
            {
                auto path = PathUtils::OpenFileDialog(false, OFN_EXPLORER | OFN_FILEMUSTEXIST,
                                                      "Keyframes (*.json)\0*.json\0", "json");
                if (path.has_value())
                {
                    Components::KeyframeSerializer::Read(path.value());
                    LOG_INFO("Read keyframes from {}", path.value().string());
                }
            }
        }
    }

    void KeyframeEditor::Render()
    {
        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::ControlBar)->GetPosition().y +
                           UIManager::Get().GetUIComponent(UI::Component::ControlBar)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - GetPosition().y);

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        if (Components::CaptureManager::Get().IsCapturing())
            return;

        const auto padding = ImGui::GetStyle().WindowPadding;

        const auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
        auto currentTick = demoInfo.currentTick;
        const auto& propertyKeyframes = Components::KeyframeManager::Get().GetKeyframes();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse;
        if (ImGui::Begin("Keyframe Editor", nullptr, flags))
        {
            ImGui::SetCursorPos(padding);

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));

            if (ImGui::BeginTable("##keyframeEditorTableLayout", 2, ImGuiTableFlags_SizingFixedFit))
            {
                auto firstColumnSize = ImGui::GetFontSize() * 1.4f + GetSize().x / 8 + padding.x * 3;

                ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_NoSort, firstColumnSize);
                ImGui::TableSetupColumn("Keyframes", ImGuiTableColumnFlags_NoSort, GetSize().x / 8 * 7);

                for (const auto& pair : propertyKeyframes)
                {
                    const auto& property = pair.first;
                    const auto& keyframes = pair.second;

                    if (!keyframes.empty() && !propertyVisible[property])
                        propertyVisible[property] = true;

                    if (!propertyVisible[property])
                        continue;

                    ImGui::TableNextRow();

                    ImGui::SetNextItemWidth(GetSize().x / 8);
                    ImGui::TableSetColumnIndex(0);
                    auto showCurve = ImGui::TreeNode(property.name.data());
                    if (showCurve)
                    {
                        for (int i = 0; i < property.GetValueCount(); i++)
                        {
                            auto startY = ImGui::GetCursorPosY();
                            auto disableValueInput = keyframes.empty();

                            ImGui::SetCursorPosY(startY + CURVE_EDITOR_LANE_HEIGHT / 8);
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding.x);
                            auto value =
                                Components::KeyframeManager::Get().Interpolate(property, keyframes, currentTick);
                            ImGui::Text(Types::KeyframeValue::GetValueIndexName(property.valueType, i).data());

                            if (disableValueInput)
                                ImGui::BeginDisabled();

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding.x);
                            DrawKeyframeValueInput(std::format("##valueInput{0}{1}", property.name, i).c_str(),
                                                   value.GetByIndex(i), property, i);
                            DrawLeftArrow(std::format("##leftArrow{0}{1}", property.name, i).c_str(), property);
                            DrawRightArrow(std::format("##rightArrow{0}{1}", property.name, i).c_str(), property);

                            ImGui::SameLine();

                            ImGui::SetCursorPosX(GetSize().x / 8);
                            DrawVerticalZoomButtons(property, i);

                            if (disableValueInput)
                                ImGui::EndDisabled();

                            ImGui::SetCursorPosY(startY + CURVE_EDITOR_LANE_HEIGHT + ImGui::GetStyle().ItemSpacing.y);
                        }
                    }

                    auto progressBarWidth = GetSize().x - firstColumnSize - GetSize().x * 0.05f - padding.x * 2;
                    ImGui::SetNextItemWidth(progressBarWidth);
                    ImGui::TableSetColumnIndex(1);
                    DrawKeyframeSlider(property);

                    if (showCurve)
                    {
                        DrawCurveEditor(property, progressBarWidth);
                        ImGui::TreePop();
                    }
                }

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);

                constexpr auto PROPERTY_SELECT_POPUP__LABEL = "##selectKeyframedProperties";
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GetSize().x / 28);
                if (ImGui::Button(ICON_FA_PLUS " Add Property", ImVec2(GetSize().x / 14, 0)))
                {
                    ImGui::OpenPopup(PROPERTY_SELECT_POPUP__LABEL);
                }

                if (ImGui::BeginPopup(PROPERTY_SELECT_POPUP__LABEL))
                {
                    for (auto& [property, _] : propertyKeyframes)
                    {
                        ImGui::MenuItem(property.name.data(), "", &propertyVisible[property]);
                    }

                    ImGui::EndPopup();
                }

                ImGui::EndTable();
            }

            auto miscButtonsY = ImGui::GetWindowHeight() - ImGui::GetFontSize() * 2 - padding.y;
            if (miscButtonsY > ImGui::GetCursorPosY())
            {
                auto hasKeyframes = std::any_of(propertyKeyframes.begin(), propertyKeyframes.end(),
                                                [](const auto& pair) { return !pair.second.empty(); });
                DrawMiscButtons(padding, hasKeyframes);
            }

            if (ImGui::BeginPopupModal(CLEAR_KEYFRAMES_POPUP_LABEL, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("This will delete all currently placed keyframes");
                if (ImGui::Button("Delete"))
                {
                    Components::KeyframeManager::Get().ClearKeyframes();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Keep"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::PopStyleVar();

            ImGui::End();
        }
    }

    void KeyframeEditor::Release()
    {
    }
}  // namespace IWXMVM::UI
