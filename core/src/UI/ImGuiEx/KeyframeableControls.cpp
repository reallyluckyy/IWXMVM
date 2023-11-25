#include "StdInclude.hpp"
#include "KeyframeableControls.hpp"

#include "Components/KeyframeManager.hpp"
#include "Mod.hpp"
#include "Resources.hpp"

namespace ImGuiEx::Keyframeable
{
    void DrawTooltip()
    {
        if (ImGui::BeginItemTooltip())
        {
            ImGui::TextUnformatted("This property is being controlled by the keyframe editor");
            ImGui::EndTooltip();
        }
    }

    void DrawKeyframeButton(const char* label, bool drawButton, std::function<void()> AddKeyframe)
    {
        if (drawButton)
        {
            if (ImGui::Button(std::format(ICON_FA_STOPWATCH "##{0}", label).c_str(), 
                ImVec2(ImGui::GetFontSize() * 1.5, ImGui::GetFontSize() * 1.5)))
            {
                AddKeyframe();
            }
            ImGui::SameLine();
        }
        else
        {
            ImGui::Dummy(ImVec2(ImGui::GetFontSize() * 1.5, ImGui::GetFontSize() * 1.5));
            ImGui::SameLine();
        }
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, 
        IWXMVM::Types::KeyframeablePropertyType propertyType)
    {
        using namespace IWXMVM;

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(propertyType);

        if (property.valueType != Types::KeyframeValueType::FloatingPoint)
            throw std::invalid_argument("Property is not a floating point value");

        const auto& keyframes = keyframeManager.GetKeyframes(property);
        DrawKeyframeButton(label, keyframes.empty(), [&]() {
            keyframeManager.GetKeyframes(property).emplace_back(
                property, 
                IWXMVM::Mod::GetGameInterface()->GetDemoInfo().currentTick,
                *v
            );
        });

        ImGui::Text(label);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4);

        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::SliderFloat(std::format("##{0}", label).c_str(), v, v_min, v_max);
        }
        else
        {
            auto interpolatedValue =
                keyframeManager.Interpolate(property, Mod::GetGameInterface()->GetDemoInfo().currentTick);

            *v = interpolatedValue.floatingPoint;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
            ImGui::SliderFloat(std::format("##{0}", label).c_str(), v, v_min, v_max, "%.2f", ImGuiSliderFlags_NoInput);
            ImGui::PopStyleVar();

            DrawTooltip();
        }
        return result;
    }

    bool ColorEdit3(const char* label, float col[3], IWXMVM::Types::KeyframeablePropertyType propertyType)
    {
        using namespace IWXMVM;

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(propertyType);

        if (property.valueType != Types::KeyframeValueType::Vector3)
            throw std::invalid_argument("Property is not a vector3 value");

        const auto& keyframes = keyframeManager.GetKeyframes(property);
        DrawKeyframeButton(label, keyframes.empty(), [&]() {
            keyframeManager.GetKeyframes(property).emplace_back(
                property, 
                IWXMVM::Mod::GetGameInterface()->GetDemoInfo().currentTick, 
                glm::vec3(col[0], col[1], col[2])
            );
        });

        ImGui::Text(label);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6 - ImGui::GetStyle().WindowPadding.x);
        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::ColorEdit3(std::format("##{0}", label).c_str(), col);
        }
        else
        {
            auto interpolatedValue =
                keyframeManager.Interpolate(property, Mod::GetGameInterface()->GetDemoInfo().currentTick);

            col[0] = interpolatedValue.vector3.x;
            col[1] = interpolatedValue.vector3.y;
            col[2] = interpolatedValue.vector3.z;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
            ImGui::ColorEdit3(std::format("##{0}", label).c_str(), col);
            ImGui::PopStyleVar();

            DrawTooltip();
        }
        return result;
    }

    bool SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max,
                     IWXMVM::Types::KeyframeablePropertyType propertyType)
    {
        using namespace IWXMVM;

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(propertyType);

        if (property.valueType != Types::KeyframeValueType::FloatingPoint)
            throw std::invalid_argument("Property is not a floating point value");

        const auto& keyframes = keyframeManager.GetKeyframes(property);
        DrawKeyframeButton(label, keyframes.empty(), [&]() {
            keyframeManager.GetKeyframes(property).emplace_back(
                property, IWXMVM::Mod::GetGameInterface()->GetDemoInfo().currentTick,
                glm::degrees(*v_rad)
            );
        });

        ImGui::Text(label);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6 - ImGui::GetStyle().WindowPadding.x);
        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::SliderAngle(std::format("##{0}", label).c_str(), v_rad, v_degrees_min, v_degrees_max);
        }
        else
        {
            auto interpolatedValue =
                keyframeManager.Interpolate(property, Mod::GetGameInterface()->GetDemoInfo().currentTick);

            *v_rad = glm::radians(interpolatedValue.floatingPoint);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
            ImGui::SliderAngle(std::format("##{0}", label).c_str(), v_rad, v_degrees_min, v_degrees_max);
            ImGui::PopStyleVar();

            DrawTooltip();
        }
        return result;
    }
}