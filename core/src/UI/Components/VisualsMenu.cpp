#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/KeyframeableControls.hpp"
#include "Events.hpp"
#include "Resources.hpp"

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
            auto filmtweaks = Mod::GetGameInterface()->GetFilmtweaks();

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
                       0,  
                       0,
                       sun.brightness,
                       filmtweaks.enabled,
                       filmtweaks.brightness,
                       filmtweaks.contrast,
                       filmtweaks.desaturation,
                       glm::make_vec3(filmtweaks.tintLight),
                       glm::make_vec3(filmtweaks.tintDark),
                       filmtweaks.invert};

            SetAngleFromPosition(sun.direction);

            visualsInitialized = true;
        });
    }

    void VisualsMenu::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Visuals", NULL, flags))
        {
            if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            {
                ImGui::Text("Load a demo to control visual settings");
                ImGui::End();
                return;
            }

            RenderConfigSection();
            RenderMiscSection();

            RenderSun();
            RenderFilmtweaks();
            RenderDOF();

            ImGui::End();
        }
    }

    void VisualsMenu::RenderConfigSection()
    {
        if (ImGui::BeginCombo("##configCombo", "Default"))
        {
            if (ImGui::Selectable(ICON_FA_CHESS_KING " Default Config", true))
            {
                // TODO: Restore default settings
            }

            ImGui::Separator();
            ImGui::Selectable(ICON_FA_ARROW_ROTATE_RIGHT " config1.cfg", false);
            ImGui::Selectable(ICON_FA_ARROW_ROTATE_RIGHT " config2.cfg", false);
            
            ImGui::Separator();
            if (ImGui::Selectable(ICON_FA_FOLDER_OPEN " Load from file", false))
            {
                // TODO: open file browse dialog
            }

            ImGui::EndCombo();
        }

        static bool configModified = false;
        if (!configModified)
            ImGui::BeginDisabled();

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save"))
        {
            // TODO: save changed to current config
        }

        if (!configModified)
            ImGui::EndDisabled();

        ImGui::Separator();
    }

    void VisualsMenu::RenderMiscSection()
    {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("Misc");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Remove HUD");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        ImGui::Checkbox("##removeHudCheckbox", &visuals.removeHud);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Remove Hitmarker");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        ImGui::Checkbox("##removeHitmarkerCheckbox", &visuals.removeHitmarker);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Remove Score");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        ImGui::Checkbox("##removeScoreCheckbox", &visuals.removeScore);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Remove Flashbang");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        ImGui::Checkbox("##removeFlashbangCheckbox", &visuals.removeFlashbang);

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Remove Killfeed");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        ImGui::Checkbox("##removeKillfeedCheckbox", &visuals.removeKillfeed);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Team 1 Color");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        static glm::vec3 team1Color;
        ImGui::ColorEdit3("##team1Color", glm::value_ptr(team1Color));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Team 2 Color");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        static glm::vec3 team2Color;
        ImGui::ColorEdit3("##team2Color", glm::value_ptr(team2Color));

        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::Separator();
    }

    void VisualsMenu::RenderFilmtweaks()
    {
        ImGui::AlignTextToFramePadding();

        ImGui::Text("Filmtweaks");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Enable Filmtweaks");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        if(ImGui::Checkbox("##enableFilmtweaksCheckbox", &visuals.filmtweaksActive))
            UpdateFilmtweaks();

        // TODO: Make these all keyframable
        if (ImGui::SliderFloat("Brightness", &visuals.filmtweaksBrightness, -1, 1))
            UpdateFilmtweaks();
        if (ImGui::SliderFloat("Contrast", &visuals.filmtweaksContrast, 0, 4))
            UpdateFilmtweaks();
        if (ImGui::SliderFloat("Desaturation", &visuals.filmtweaksDesaturation, 0, 4))
            UpdateFilmtweaks();
        if (ImGui::ColorEdit3("Tint Light", glm::value_ptr(visuals.filmtweaksTintLight)))
            UpdateFilmtweaks();
        if (ImGui::ColorEdit3("Tint Dark", glm::value_ptr(visuals.filmtweaksTintDark)))
            UpdateFilmtweaks();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Invert");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        if(ImGui::Checkbox("##invertFilmtweaksCheckbox", &visuals.filmtweaksInvert))
            UpdateFilmtweaks();

        ImGui::Separator();
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

        if (ImGuiEx::Keyframeable::ColorEdit3("Color", glm::value_ptr(visuals.sunColorUI),
                                              Types::KeyframeablePropertyType::SunLightColor))
            UpdateSun();

        if (ImGuiEx::Keyframeable::SliderFloat("Brightness", &visuals.sunBrightness, 0, 4,
                                             Types::KeyframeablePropertyType::SunLightBrightness))
            UpdateSun();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        // ImGui::SliderAngle sets sun angles in radians but displays them as degrees
        if (ImGuiEx::Keyframeable::SliderAngle("Pitch", &visuals.sunPitch, 0, 360,
                                               Types::KeyframeablePropertyType::SunLightPitch))
            UpdateSunAngle();

        if (ImGuiEx::Keyframeable::SliderAngle("Yaw", &visuals.sunYaw, 0, 360,
                                               Types::KeyframeablePropertyType::SunLightYaw))
            UpdateSunAngle();

        /*
        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        if (ImGui::SliderFloat("X", glm::value_ptr(visuals.sunDirectionUI), -1, 1))
            UpdateSun();

        if (ImGui::SliderFloat("Y", &(glm::value_ptr(visuals.sunDirectionUI)[1]), -1, 1))
            UpdateSun();

        if (ImGui::SliderFloat("Z", &(glm::value_ptr(visuals.sunDirectionUI)[2]), -1, 1))
            UpdateSun();
        */

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

    void VisualsMenu::SetAngleFromPosition(glm::vec3 pos)
    {
        auto norm = glm::normalize(pos);

        float pitch = glm::asin(norm.y);
        float yaw = glm::atan(norm.z, norm.x);

        visuals.sunPitch = glm::mod(pitch + 2.0f * glm::pi<float>(), 2.0f * glm::pi<float>());
        visuals.sunYaw = glm::mod(yaw + 2.0f * glm::pi<float>(), 2.0f * glm::pi<float>());
    }

    void VisualsMenu::UpdateFilmtweaks()
    {
        
        Types::Filmtweaks filmtweakSettings = {visuals.filmtweaksActive, visuals.filmtweaksBrightness, visuals.filmtweaksContrast, 
            visuals.filmtweaksDesaturation, visuals.filmtweaksTintLight, visuals.filmtweaksTintDark, visuals.filmtweaksInvert};
        Mod::GetGameInterface()->SetFilmtweaks(filmtweakSettings);
    }

    void VisualsMenu::Release()
    {
    }
}  // namespace IWXMVM::UI