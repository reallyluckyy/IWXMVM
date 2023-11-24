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

    void DrawContextMenu(IWXMVM::Types::KeyframeablePropertyType propertyType)
    {
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            ImGui::OpenPopup("##keyframeablePropertyContextMenu");

        if (ImGui::BeginPopupContextItem("##keyframeablePropertyContextMenu"))
        {
            if (ImGui::MenuItem("Add Keyframe"))
            {
                auto& keyframeManager = IWXMVM::Components::KeyframeManager::Get();
                const auto& property = keyframeManager.GetProperty(propertyType);

                keyframeManager.GetKeyframes(property).emplace_back(
                    property,
                    IWXMVM::Mod::GetGameInterface()->GetDemoInfo().currentTick, 
                    IWXMVM::Types::KeyframeValue::GetDefaultValue(property.valueType)
                );
            }

            ImGui::EndPopup();
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

        bool result = true;
        if (keyframes.empty())
        {
            result = ImGui::SliderFloat(label, v, v_min, v_max);
        }
        else
        {
            auto interpolatedValue =
                keyframeManager.Interpolate(property, Mod::GetGameInterface()->GetDemoInfo().currentTick);

            *v = interpolatedValue.floatingPoint;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
            ImGui::SliderFloat(label, v, v_min, v_max, "%.3f", ImGuiSliderFlags_NoInput);
            ImGui::PopStyleVar();

            DrawTooltip();
        }

        DrawContextMenu(propertyType);
        return result;
    }
}