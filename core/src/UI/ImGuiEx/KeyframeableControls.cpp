#include "StdInclude.hpp"
#include "KeyframeableControls.hpp"

#include "Components/KeyframeManager.hpp"
#include "Mod.hpp"
#include "Resources.hpp"

namespace ImGuiEx::Keyframeable
{
    bool SliderFloat(const char* label, float* v, float v_min, float v_max, 
        IWXMVM::Types::KeyframeablePropertyType propertyType)
    {
        using namespace IWXMVM;

        auto& keyframeManager = Components::KeyframeManager::Get();
        const auto& property = keyframeManager.GetProperty(propertyType);

        if (property.valueType != Types::KeyframeValueType::FloatingPoint)
            throw std::invalid_argument("Property is not a floating point value");

        const auto& keyframes = keyframeManager.GetKeyframes(property);

        if (keyframes.empty())
            return ImGui::SliderFloat(label, v, v_min, v_max);

        auto interpolatedValue =
            keyframeManager.Interpolate(property, Mod::GetGameInterface()->GetDemoInfo().currentTick);

        *v = interpolatedValue.floatingPoint;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        ImGui::SliderFloat(label, v, v_min, v_max, "%.3f", ImGuiSliderFlags_NoInput);
        ImGui::PopStyleVar();

        if (ImGui::BeginItemTooltip())
        {
            ImGui::TextUnformatted("This property is being controlled by the keyframe editor");
            ImGui::EndTooltip();
        }

        // add circle next to slider
        auto sliderHeight = ImGui::GetItemRectSize().y;
        const auto circlePos = ImGui::GetItemRectMax() - ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y + sliderHeight / 2);
        ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), circlePos, ImGui::GetColorU32(ImGuiCol_Button), ICON_FA_DIAMOND);

        return true;
    }
}