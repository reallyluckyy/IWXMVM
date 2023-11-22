
#include "StdInclude.hpp"
#include "KeyframeEditor.hpp"

#include <IconsFontAwesome6.h>
#include <imgui_internal.h>

#include "UI/UIManager.hpp"
#include "UI/ImGuiExtensions.hpp"
#include "Mod.hpp"
#include "Input.hpp"
#include "Components/KeyframeManager.hpp"
#include "Events.hpp"

namespace IWXMVM::UI
{
    std::tuple<int32_t, int32_t> KeyframeEditor::GetDisplayTickRange() const
    {
        return {displayStartTick, displayEndTick};
    }

    void KeyframeEditor::HandleTimelineZoomInteractions(float barLength)
    {
        const auto ZOOM_MULTIPLIER = 2000;
        const auto MOVE_MULTIPLIER = 1;

        const int32_t MINIMUM_ZOOM = 500;

        const auto minTick = 0;
        const auto maxTick = (int32_t)Mod::GetGameInterface()->GetDemoInfo().endTick;

        auto scrollDelta = Input::GetScrollDelta() * Input::GetDeltaTime();
        displayStartTick += scrollDelta * 100 * ZOOM_MULTIPLIER;
        displayStartTick = glm::clamp(displayStartTick, minTick, displayEndTick - MINIMUM_ZOOM);
        displayEndTick -= scrollDelta * 100 * ZOOM_MULTIPLIER;
        displayEndTick = glm::clamp(displayEndTick, displayStartTick + MINIMUM_ZOOM, maxTick);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            auto zoomWindowSizeBefore = displayEndTick - displayStartTick;

            auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            delta /= barLength;
            delta *= (displayEndTick - displayStartTick);

            displayStartTick = glm::max(displayStartTick - (int32_t)(delta.x * MOVE_MULTIPLIER), minTick);
            displayEndTick = glm::min(displayEndTick - (int32_t)(delta.x * MOVE_MULTIPLIER), maxTick);

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

    ImVec2 GetPositionForKeyframe(
        const ImRect rect, const Types::Keyframe& keyframe, const uint32_t startTick, const uint32_t endTick,
        const ImVec2 valueBoundaries = ImVec2(-10, 10), int32_t valueIndex = 0)
    {
        const auto barHeight = rect.Max.y - rect.Min.y;
        const auto barLength = rect.Max.x - rect.Min.x;
        const auto tickPercentage = static_cast<float>(keyframe.tick - startTick) / static_cast<float>(endTick - startTick);
        const auto x = rect.Min.x + tickPercentage * barLength;
        const auto valuePercentage =
            1 - (keyframe.value.GetByIndex(valueIndex) - valueBoundaries.x) / (valueBoundaries.y - valueBoundaries.x);
        const auto y = rect.Min.y + valuePercentage * barHeight;
        return ImVec2(x, y);
    };

    std::tuple<uint32_t, Types::KeyframeValue> GetKeyframeForPosition(const ImVec2 position, const ImRect rect,
                                                                      const uint32_t startTick, const uint32_t endTick,
                                                                      const ImVec2 valueBoundaries = ImVec2(-10, 10))
    {
        const auto barHeight = rect.Max.y - rect.Min.y;
        const auto barLength = rect.Max.x - rect.Min.x;
        const auto tickPercentage = (position.x - rect.Min.x) / barLength;
        const auto tick = static_cast<uint32_t>(startTick + tickPercentage * (endTick - startTick));
        const auto valuePercentage = (position.y - rect.Min.y) / barHeight;
        const auto value = valueBoundaries.x + (1 - valuePercentage) * (valueBoundaries.y - valueBoundaries.x);
        return std::make_tuple(tick, value);
    };

    void SortKeyframes(std::vector<Types::Keyframe>& keyframes)
    {
        std::sort(keyframes.begin(), keyframes.end(), [](const auto& a, const auto& b) { return a.tick < b.tick; });
    }

    std::optional<int32_t> selectedKeyframeId = std::nullopt;
    std::optional<int32_t> selectedKeyframeValueIndex = std::nullopt;


    void KeyframeEditor::DrawKeyframeSliderInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                                    uint32_t displayStartTick, uint32_t displayEndTick,
                                                    std::vector<Types::Keyframe>& keyframes)
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

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, displayStartTick, displayEndTick);

        bool isAnyKeyframeHovered = false;


        const auto textSize = CalcTextSize(ICON_FA_DIAMOND);
        for (auto it = keyframes.begin(); it != keyframes.end();)
        {
            auto& k = *it;

            const auto position = GetPositionForKeyframe(frame_bb, k, displayStartTick, displayEndTick);

            const auto barHeight = frame_bb.Max.y - frame_bb.Min.y;
            ImRect text_bb(ImVec2(position.x - textSize.x / 2, frame_bb.Min.y + (barHeight - textSize.y) / 2),
                           ImVec2(position.x + textSize.x / 2, frame_bb.Max.y - (barHeight - textSize.y) / 2));
            const bool hovered = ItemHoverable(text_bb, id, g.LastItemData.InFlags);

            isAnyKeyframeHovered |= hovered;

            if (hovered && IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedKeyframeId = k.id;
                selectedKeyframeValueIndex = std::nullopt;
            }

            const ImU32 col = GetColorU32(hovered || selectedKeyframeId == k.id ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
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
                SortKeyframes(keyframes);

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
            HandleTimelineZoomInteractions(frame_bb.Max.x - frame_bb.Min.x);
        }

        if (hovered && !isAnyKeyframeHovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            auto [tick, _] = GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick);

            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) { return k.tick == tick; }) ==
                keyframes.end())
            {
                // TODO: the value here should be the interpolated value between neighboring keyframes
                keyframes.emplace_back(property, tick, Types::KeyframeValue(0));

                SortKeyframes(keyframes);
            }
        }
    }

    void KeyframeEditor::DrawCurveEditorInternal(const Types::KeyframeableProperty& property, uint32_t* currentTick,
                                                 uint32_t displayStartTick, uint32_t displayEndTick, const float width,
                                                 std::vector<Types::Keyframe>& keyframes, int32_t keyframeValueIndex)
    {
        using namespace ImGui;

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
        float halfNeedleWidth = 2;

        auto t =
            static_cast<float>(*currentTick - displayStartTick) / static_cast<float>(displayEndTick - displayStartTick);
        ImRect grab_bb = ImRect(frame_bb.Min.x + t * width - halfNeedleWidth, frame_bb.Min.y,
                                frame_bb.Min.x + t * width + halfNeedleWidth, frame_bb.Max.y);

        if (grab_bb.Max.x > grab_bb.Min.x)
            window->DrawList->AddRectFilled(
                grab_bb.Min, grab_bb.Max,
                GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        ImGuiEx::DemoProgressBarLines(frame_bb, *currentTick, displayStartTick, displayEndTick);

        bool isAnyKeyframeHovered = false;

        const auto textSize = CalcTextSize(ICON_FA_DIAMOND);

        ImVec2 valueBoundaries;
        if (keyframes.empty())
        {
            valueBoundaries = ImVec2(-10, 10);
        }
        else
        {
            const auto minValueKeyframe = std::min_element(
                keyframes.begin(), keyframes.end(),
                [keyframeValueIndex](const auto& a, const auto& b) { return a.value.GetByIndex(keyframeValueIndex) < b.value.GetByIndex(keyframeValueIndex); });

            const auto maxValueKeyframe = std::max_element(
                keyframes.begin(), keyframes.end(),
                [keyframeValueIndex](const auto& a, const auto& b) { return a.value.GetByIndex(keyframeValueIndex) < b.value.GetByIndex(keyframeValueIndex); });

            auto minValue = minValueKeyframe->value.GetByIndex(keyframeValueIndex);
            auto maxValue = maxValueKeyframe->value.GetByIndex(keyframeValueIndex);
            valueBoundaries = ImVec2(minValue < 0 ? minValue * 1.15f : minValue / 1.15f,
                                     maxValue < 0 ? maxValue / 1.15f : maxValue * 1.15f);
            valueBoundaries += ImVec2(-10, 10);
        }

        for (auto& k : keyframes)
        {
            const auto position = GetPositionForKeyframe(frame_bb, k, displayStartTick, displayEndTick, valueBoundaries, keyframeValueIndex);

            ImRect text_bb(ImVec2(position.x - textSize.x / 2, position.y - textSize.y / 2),
                           ImVec2(position.x + textSize.x / 2, position.y + textSize.y / 2));
            const bool hovered = ItemHoverable(text_bb, id, g.LastItemData.InFlags);

            isAnyKeyframeHovered |= hovered;

            if (hovered && IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedKeyframeId = k.id;
                selectedKeyframeValueIndex = keyframeValueIndex;
            }

            const ImU32 col = GetColorU32(hovered || selectedKeyframeId == k.id ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            window->DrawList->AddText(text_bb.Min, col, ICON_FA_DIAMOND);
            window->DrawList->AddText(ImVec2(text_bb.Max.x, text_bb.Min.y), GetColorU32(ImGuiCol_Text),
                                      std::format("{0:.0f}", k.value.GetByIndex(keyframeValueIndex)).c_str());

            if (selectedKeyframeId == k.id && selectedKeyframeValueIndex == keyframeValueIndex)
            {
                auto [tick, value] =
                    GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick, valueBoundaries);

                k.tick = tick;

                auto currentValue = k.value.GetByIndex(keyframeValueIndex);
                auto maxNewValue = glm::min(glm::abs(currentValue) * 1.25f + 1, glm::abs(currentValue) + 5.0f);
                if (glm::abs(value.floatingPoint) > maxNewValue)
                    value.floatingPoint = glm::sign(value.floatingPoint) * maxNewValue;
                k.value.SetByIndex(keyframeValueIndex, glm::fclamp(value.floatingPoint, -100'000.0f, 100'000.0f));
                SortKeyframes(keyframes);

                if (IsMouseReleased(ImGuiMouseButton_Left))
                {
                    selectedKeyframeId = std::nullopt;
                    selectedKeyframeValueIndex = std::nullopt;
                }
            }
        }

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
                auto position = GetPositionForKeyframe(frame_bb, Types::Keyframe(property, tick, value), displayStartTick, displayEndTick,
                                                        valueBoundaries, keyframeValueIndex);
                polylinePoints.push_back(position);
            }

            window->DrawList->AddPolyline(polylinePoints.data(), polylinePoints.size(), GetColorU32(ImGuiCol_Button), 0,
                                          2.0f);
        }

        const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
        if (hovered && !isAnyKeyframeHovered && !selectedKeyframeId.has_value())
        {
            HandleTimelineZoomInteractions(frame_bb.Max.x - frame_bb.Min.x);
        }

        if (hovered && IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            auto [tick, value] =
                GetKeyframeForPosition(GetMousePos(), frame_bb, displayStartTick, displayEndTick, valueBoundaries);
            if (std::find_if(keyframes.begin(), keyframes.end(), [tick](const auto& k) { return k.tick == tick; }) ==
                keyframes.end())
            {
                keyframes.emplace_back(property, tick, value);
                SortKeyframes(keyframes);
            }
        }
    }

    void KeyframeEditor::Initialize()
    {
        Events::RegisterListener(EventType::OnDemoLoad, [this]() {
            displayStartTick = 0;
            displayEndTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
        });

        for (const auto& pair : Components::KeyframeManager::Get().GetKeyframes())
            propertyVisible[pair.first] = false;
    }

    void KeyframeEditor::DrawKeyframeSlider(const Types::KeyframeableProperty& property)
    {
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        auto [displayStartTick, displayEndTick] = GetDisplayTickRange();

        DrawKeyframeSliderInternal(property, &currentTick, displayStartTick, displayEndTick,
                                   Components::KeyframeManager::Get().GetKeyframes(property));
    }

    void KeyframeEditor::DrawCurveEditor(const Types::KeyframeableProperty& property, const auto width)
    {
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        auto [displayStartTick, displayEndTick] = GetDisplayTickRange();

        // TODO: Probably disable for CameraData, as I'm not sure if this is helpful to the user at all
        
        // if (property.valueType == Types::KeyframeValueType::CameraData)
        //    return;

        // I dont really like the way this is done, I just cant really think of a better solution right now
        auto GetValueCountForProperty = [](auto property) -> int32_t {
            switch (property.valueType)
            {
                case Types::KeyframeValueType::FloatingPoint:
                    return 1;
                case Types::KeyframeValueType::CameraData:
                    return 7;
                case Types::KeyframeValueType::Vector3:
                    return 3;
                default:
                    throw std::runtime_error("Not implemented");
            }
        };

        for (int i = 0; i < GetValueCountForProperty(property); i++)
        {
            DrawCurveEditorInternal(
                property, &currentTick, displayStartTick, displayEndTick, width,
                Components::KeyframeManager::Get().GetKeyframes(property), i
            );
        }
    }

    void KeyframeEditor::Render()
    {
        SetPosition(0, UIManager::Get().GetUIComponent(UI::Component::ControlBar)->GetPosition().y +
                           UIManager::Get().GetUIComponent(UI::Component::ControlBar)->GetSize().y);
        SetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - GetPosition().y);

        ImGui::SetNextWindowPos(GetPosition());
        ImGui::SetNextWindowSize(GetSize());

        const auto padding = ImGui::GetStyle().WindowPadding;

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

                const auto& propertyKeyframes = Components::KeyframeManager::Get().GetKeyframes();
                for (const auto& pair : propertyKeyframes)
                {
                    const auto& property = pair.first;
                    const auto& keyframes = pair.second;

                    if (!propertyVisible[property] && keyframes.empty())
                        continue;

                    ImGui::TableNextRow();

                    ImGui::SetNextItemWidth(GetSize().x / 8);
                    ImGui::TableSetColumnIndex(0);
                    auto showCurve = ImGui::TreeNode(property.name.data());

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

                constexpr auto POPUP_LABEL = "##selectKeyframedProperties";
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GetSize().x / 28);
                if (ImGui::Button(ICON_FA_PLUS " Add Property", ImVec2(GetSize().x / 14, 0)))
                {
                    ImGui::OpenPopup(POPUP_LABEL);
                }

                if (ImGui::BeginPopup(POPUP_LABEL))
                {
                    for (auto& [property, _] : propertyKeyframes)
                    {
                        ImGui::MenuItem(property.name.data(), "", &propertyVisible[property]);
                    }

                    ImGui::EndPopup();
                }

                ImGui::EndTable();
            }

            ImGui::PopStyleVar();

            ImGui::End();
        }
    }

    void KeyframeEditor::Release()
    {
    }
}  // namespace IWXMVM::UI
