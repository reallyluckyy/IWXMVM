#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/KeyframeableControls.hpp"
#include "Events.hpp"

namespace IWXMVM::UI
{
    void VisualsMenu::Initialize()
    {
        // TODO: Come up with a proper plan on when we want to initialize the UI values from the games values
        // TODO: Ideally we dont want to ever overwrite a users custom settings, but what if the map changes,
        // TODO: the user hasnt edited the sun settings yet, and suddenly the sun is in a different position?

        Events::RegisterListener(EventType::OnDemoLoad, [&]() {
            if (visualsInitialized)
                return;

            auto dof = Mod::GetGameInterface()->GetDof();
            auto sun = Mod::GetGameInterface()->GetSun();

            visuals = {dof.enabled,
                       dof.farBlur,
                       dof.farStart,
                       dof.farEnd,
                       dof.nearBlur,
                       dof.nearStart,
                       dof.nearEnd,
                       dof.bias,
                       glm::make_vec3(sun.color),
                       glm::make_vec3(sun.direction),
                       0,  // TODO: initialize pitch and yaw
                       0,
                       sun.brightness};

            visualsInitialized = true;
        });
    }

    void VisualsMenu::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Visuals", NULL, flags))
        {
            RenderDOF();
            RenderSun();

            ImGui::End();
        }
    }

    void VisualsMenu::RenderDOF()
    {
        ImGui::AlignTextToFramePadding();

        ImGui::Text("Depth of Field");

        if (ImGui::Checkbox("Enable DOF", &visuals.dofActive))
            UpdateDof();

        if (ImGui::SliderFloat("Far Blur", &visuals.dofFarBlur, 0, 10))
            UpdateDof();

        if (ImGui::SliderFloat("Far Start", &visuals.dofFarStart, 0, 5000))
            UpdateDof();

        if (ImGui::SliderFloat("Far End", &visuals.dofFarEnd, 0, 5000))
            UpdateDof();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        if (ImGui::SliderFloat("Near Blur", &visuals.dofNearBlur, 0, 10))
            UpdateDof();

        if (ImGui::SliderFloat("Near Start", &visuals.dofNearStart, 0, 5000))
            UpdateDof();

        if (ImGui::SliderFloat("Near End", &visuals.dofNearEnd, 0, 5000))
            UpdateDof();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        if (ImGui::SliderFloat("Bias", &visuals.dofBias, 0.1, 10))
            UpdateDof();

        ImGui::Separator();
    }

    void VisualsMenu::RenderSun()
    {
        ImGui::AlignTextToFramePadding();

        ImGui::Text("Sun");

        if (ImGui::ColorEdit3("Color", glm::value_ptr(visuals.sunColorUI)))
            UpdateSun();

        if (ImGuiEx::Keyframeable::SliderFloat("Brightness", &visuals.sunBrightness, 0, 4,
                                             Types::KeyframeablePropertyType::SunLightBrightness))
            UpdateSun();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        // ImGui::SliderAngle sets sun angles in radians but displays them as degrees
        if (ImGui::SliderAngle("Pitch", &visuals.sunPitch, 0, 360))
            UpdateSunAngle();

        if (ImGui::SliderAngle("Yaw", &visuals.sunYaw, 0, 360))
            UpdateSunAngle();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        if (ImGui::SliderFloat("X", glm::value_ptr(visuals.sunDirectionUI), -1, 1))
            UpdateSun();

        if (ImGui::SliderFloat("Y", &(glm::value_ptr(visuals.sunDirectionUI)[1]), -1, 1))
            UpdateSun();

        if (ImGui::SliderFloat("Z", &(glm::value_ptr(visuals.sunDirectionUI)[2]), -1, 1))
            UpdateSun();

        ImGui::Separator();
    }

    void VisualsMenu::UpdateDof()
    {
        Types::DoF dofSettings = {visuals.dofActive,   visuals.dofFarBlur,   visuals.dofFarStart, visuals.dofFarEnd,
                                  visuals.dofNearBlur, visuals.dofNearStart, visuals.dofNearEnd,  visuals.dofBias};
        Mod::GetGameInterface()->SetDof(dofSettings);
    }

    void VisualsMenu::UpdateSun()
    {
        IWXMVM::Types::Sun sunSettings = {glm::make_vec3(visuals.sunColorUI), glm::make_vec3(visuals.sunDirectionUI),
                                          visuals.sunBrightness};
        Mod::GetGameInterface()->SetSun(sunSettings);
    }

    void VisualsMenu::UpdateSunAngle()
    {
        float origin = 0;
        float radius = 1;
        glm::vec3 rotation = {visuals.sunPitch, visuals.sunYaw, 0};

        visuals.sunDirectionUI.x = origin + radius * glm::cos(rotation.y) * glm::cos(rotation.x);
        visuals.sunDirectionUI.y = origin + radius * glm::sin(rotation.y) * glm::cos(rotation.z);
        visuals.sunDirectionUI.z = -(origin + radius * glm::sin(rotation.x));

        UpdateSun();
    }

    void VisualsMenu::Release()
    {
    }
}  // namespace IWXMVM::UI