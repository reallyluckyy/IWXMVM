#include "StdInclude.hpp"
#include "KeyframeableControls.hpp"

#include "Components/KeyframeManager.hpp"
#include "Components/Playback.hpp"
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
        ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.0f);
        ImGui::BeginDisabled(!drawButton);
        if (ImGui::Button(std::format(ICON_FA_STOPWATCH "##{0}", label).c_str(), 
            ImVec2(ImGui::GetFontSize() * 1.5f, ImGui::GetFontSize() * 1.5f)))
        {
            AddKeyframe();
        }
        ImGui::SameLine();
        ImGui::EndDisabled();
        ImGui::PopStyleVar();
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
        DrawKeyframeButton(std::format("##{0}{1}KeyframeButton", label, magic_enum::enum_name(propertyType)).c_str(),
                           keyframes.empty(), [&]() {
                keyframeManager.AddKeyframe(
                    property,
                    Types::Keyframe(property, Components::Playback::GetTimelineTick(), *v));
        });

        ImGui::Text(label);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);

        const auto labelText = std::format("##{0}{1}Label", label, magic_enum::enum_name(propertyType));
        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::SliderFloat(labelText.c_str(), v, v_min, v_max);
        }
        else
        {
            auto interpolatedValue = keyframeManager.Interpolate(property, Components::Playback::GetTimelineTick());

            *v = interpolatedValue.floatingPoint;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
            ImGui::SliderFloat(labelText.c_str(), v, v_min, v_max, "%.2f", ImGuiSliderFlags_NoInput);
            ImGui::PopStyleVar();

            DrawTooltip();
        }
        return result;
    }

    bool SliderFloat3(const char* label, float v[3], float v_min, float v_max,
                     IWXMVM::Types::KeyframeablePropertyType propertyType)
    {
        using namespace IWXMVM;

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(propertyType);

        if (property.valueType != Types::KeyframeValueType::Vector3)
            throw std::invalid_argument("Property is not a vector3 value");

        const auto& keyframes = keyframeManager.GetKeyframes(property);
        DrawKeyframeButton(std::format("##{0}{1}KeyframeButton", label, magic_enum::enum_name(propertyType)).c_str(),
                           keyframes.empty(), [&]() {
                               keyframeManager.AddKeyframe(
                                   property, Types::Keyframe(property, Components::Playback::GetTimelineTick(),
                                                   glm::vec3(v[0], v[1], v[2])));
                           });

        ImGui::Text(label);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);

        const auto labelText = std::format("##{0}{1}Label", label, magic_enum::enum_name(propertyType));
        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::SliderFloat3(labelText.c_str(), v, v_min, v_max);
        }
        else
        {
            auto interpolatedValue = keyframeManager.Interpolate(property, Components::Playback::GetTimelineTick());

            v[0] = interpolatedValue.vector3.x;
            v[1] = interpolatedValue.vector3.y;
            v[2] = interpolatedValue.vector3.z;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
            ImGui::SliderFloat3(labelText.c_str(), v, v_min, v_max, "%.2f", ImGuiSliderFlags_NoInput);
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
            keyframeManager.AddKeyframe(property, Types::Keyframe(property, Components::Playback::GetTimelineTick(),
                                          glm::vec3(col[0], col[1], col[2])));
        });

        ImGui::Text(label);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::ColorEdit3(std::format("##{0}", label).c_str(), col);
        }
        else
        {
            auto interpolatedValue = keyframeManager.Interpolate(property, Components::Playback::GetTimelineTick());

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
}