#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"
#include "UI/ImGuiEx/KeyframeableControls.hpp"
#include "Events.hpp"
#include "Resources.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{
    void VisualsMenu::Initialize()
    {
        // TODO: Come up with a proper plan on when we want to initialize the UI values from the games values
        // TODO: Ideally we dont want to ever overwrite a users custom settings, but what if the map changes

        Events::RegisterListener(EventType::PostDemoLoad, [&]() {
            if (visualsInitialized)
                return;

            auto dof = Mod::GetGameInterface()->GetDof();
            auto sun = Mod::GetGameInterface()->GetSun();
            auto filmtweaks = Mod::GetGameInterface()->GetFilmtweaks();
            auto hudInfo = Mod::GetGameInterface()->GetHudInfo();

            visuals = {
                dof,
                glm::make_vec3(sun.color),
                glm::make_vec3(sun.direction),
                0,  
                0,
                sun.brightness,
                filmtweaks,
                hudInfo
            };
            recentPresets = {};

            SetAngleFromPosition(sun.direction);

            defaultVisuals = visuals;
            defaultPreset = {"Default"};
            currentPreset = defaultPreset;

            visualsInitialized = true;
        });

        // This is a hack to get the keyframed visuals to update when the visual tab isnt selected
        Events::RegisterListener(EventType::OnFrame, [&]() {
            if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
                return;

            if (UIManager::Get().GetSelectedTab() != Tab::Visuals)
            {
                ImGui::SetCursorPos(ImVec2(100000, 100000));
                RenderMiscSection();
                RenderSun();
                RenderFilmtweaks();
                RenderDOF();
            }
        });
    }

    void VisualsMenu::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Visuals", NULL, flags))
        {
            if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            {
                UI::DrawInaccessibleTabWarning();
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

    void DrawSectionHeader(const char* label)
    {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::PushFont(UIManager::Get().GetBoldFont());
        ImGui::Text(label);
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    void VisualsMenu::RenderConfigSection()
    {
        if (ImGui::BeginCombo("##configCombo", currentPreset.name.c_str()))
        {
            if (ImGui::Selectable(ICON_FA_CHESS_KING " Default Preset", currentPreset.name == "Default"))
            {
                visuals = defaultVisuals;
                currentPreset = defaultPreset;

                UpdateDof();
                UpdateSun();
                SetAngleFromPosition(visuals.sunDirectionUI);
                UpdateFilmtweaks();
                UpdateHudInfo();
            }

            ImGui::Separator();
            
            
            for (auto& preset : recentPresets)
            {
                auto label = std::string(ICON_FA_ARROW_ROTATE_RIGHT) + " " + preset.name + "##" + preset.path.string();
                if (ImGui::Selectable(label.c_str(), currentPreset.name == preset.name))
                {
                    LoadPreset(preset);
                }
            }
            
            ImGui::Separator();
            if (ImGui::Selectable(ICON_FA_FOLDER_OPEN " Load from file", false))
            {
                auto path =
                    PathUtils::OpenFileDialog(false, OFN_EXPLORER | OFN_FILEMUSTEXIST, "Config\0*.cfg\0All Files\0*.*\0", "cfg");
                if (path.has_value())
                {
                    Preset preset;
                    preset.name = path.value().filename().string();
                    preset.path = path.value();
                    LoadPreset(preset);
                }
            }

            ImGui::EndCombo();
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save"))
        {
            auto path = PathUtils::OpenFileDialog(
                true, OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, "Config\0*.cfg\0", "cfg");
            if (path.has_value())
            {
                Components::VisualConfiguration::Save(path.value(), visuals);
                AddPresetToRecent({path.value().filename().string(), path.value()});
            }
        }

        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Separator();
    }

    void VisualsMenu::RenderMiscSection()
    {
        auto checkboxColumnPosition = ImGui::GetWindowWidth() * 0.6f;

        DrawSectionHeader(ICON_FA_BURGER "  Misc");

        bool modified = false;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Show 2D Elements");
        ImGui::SameLine();
        ImGui::SetCursorPosX(checkboxColumnPosition);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        modified = ImGui::Checkbox("##show2DElementsCheckbox", &visuals.hudInfo.show2DElements) || modified;

        if (visuals.hudInfo.show2DElements)
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Show Player HUD");
            ImGui::SameLine();
            ImGui::SetCursorPosX(checkboxColumnPosition);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::Checkbox("##showPlayerHUDCheckbox", &visuals.hudInfo.showPlayerHUD) || modified;

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Show Shellshock/Flashbang");
            ImGui::SameLine();
            ImGui::SetCursorPosX(checkboxColumnPosition);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::Checkbox("##showFlashbangCheckbox", &visuals.hudInfo.showShellshock) || modified;

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Show Crosshair");
            ImGui::SameLine();
            ImGui::SetCursorPosX(checkboxColumnPosition);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::Checkbox("##showCrosshairCheckbox", &visuals.hudInfo.showCrosshair) || modified;
            
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Show Score");
            ImGui::SameLine();
            ImGui::SetCursorPosX(checkboxColumnPosition);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::Checkbox("##showScoreCheckbox", &visuals.hudInfo.showScore) || modified;

            ImGui::Dummy(ImVec2(0.0f, 20.0f));

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Show Killfeed");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::Checkbox("##showKillfeedCheckbox", &visuals.hudInfo.showKillfeed) || modified;

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Team 1 Color");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::ColorEdit3("##killfeedTeam1Color", glm::value_ptr(visuals.hudInfo.killfeedTeam1Color)) ||
                       modified;

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Team 2 Color");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
            modified = ImGui::ColorEdit3("##killfeedTeam2Color", glm::value_ptr(visuals.hudInfo.killfeedTeam2Color)) ||
                       modified;
        }
        
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::Separator();

        if (modified)
        {
            UpdateHudInfo();
        }
    }

    void VisualsMenu::RenderFilmtweaks()
    {
        DrawSectionHeader(ICON_FA_IMAGE "  Filmtweaks");

        bool modified = false;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Enable Filmtweaks");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        modified = ImGui::Checkbox("##enableFilmtweaksCheckbox", &visuals.filmtweaks.enabled) || modified;

        modified = ImGuiEx::Keyframeable::SliderFloat("Brightness", &visuals.filmtweaks.brightness, -1, 1, 
                                                      Types::KeyframeablePropertyType::FilmtweakBrightness) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat("Contrast", &visuals.filmtweaks.contrast, 0, 4,
                                                      Types::KeyframeablePropertyType::FilmtweakContrast) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat("Desaturation", &visuals.filmtweaks.desaturation, 0, 4, 
                                                      Types::KeyframeablePropertyType::FilmtweakDesaturation) || modified;
        modified = ImGuiEx::Keyframeable::ColorEdit3("Tint Light", glm::value_ptr(visuals.filmtweaks.tintLight), 
                                                     Types::KeyframeablePropertyType::FilmtweakTintLight) || modified;
        modified = ImGuiEx::Keyframeable::ColorEdit3("Tint Dark", glm::value_ptr(visuals.filmtweaks.tintDark),
                                                     Types::KeyframeablePropertyType::FilmtweakTintDark) || modified;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Invert");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.4f);
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.6f - ImGui::GetStyle().WindowPadding.x);
        modified = ImGui::Checkbox("##invertFilmtweaksCheckbox", &visuals.filmtweaks.invert) || modified;

        if (modified)
        {
            UpdateFilmtweaks();
        }

        ImGui::Separator();
    }

    void VisualsMenu::RenderDOF()
    {
        DrawSectionHeader(ICON_FA_CAMERA "  Depth of Field");

        bool modified = false;

        modified = ImGui::Checkbox("Enable DOF", &visuals.dof.enabled) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Far Blur", &visuals.dof.farBlur, 0, 10,
            Types::KeyframeablePropertyType::DepthOfFieldFarBlur) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Far Start", &visuals.dof.farStart, 0, 5000,
            Types::KeyframeablePropertyType::DepthOfFieldFarStart) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Far End", &visuals.dof.farEnd, 0, 5000,
            Types::KeyframeablePropertyType::DepthOfFieldFarEnd) || modified;

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Near Blur", &visuals.dof.nearBlur, 0, 10,
            Types::KeyframeablePropertyType::DepthOfFieldNearBlur) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Near Start", &visuals.dof.nearStart, 0, 5000,
            Types::KeyframeablePropertyType::DepthOfFieldNearStart) || modified;
        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Near End", &visuals.dof.nearEnd, 0, 5000,
            Types::KeyframeablePropertyType::DepthOfFieldNearEnd) || modified;

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        modified = ImGuiEx::Keyframeable::SliderFloat(
            "Bias", &visuals.dof.bias, 0.1f, 10,
            Types::KeyframeablePropertyType::DepthOfFieldBias) || modified;

        if (modified)
        {
            UpdateDof();
        }

        ImGui::Separator();
    }

    bool DrawResettableSectionHeader(const char* label)
    {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::PushFont(UIManager::Get().GetBoldFont());
        ImGui::Text(label);
        ImGui::PopFont();

        const char* resetlabel = ICON_FA_REPEAT " Reset ";
        float windowWidth = ImGui::GetWindowWidth();
        float buttonWidth = ImGui::CalcTextSize(resetlabel).x + ImGui::GetStyle().FramePadding.x * 2.0f;
        ImGui::SameLine();
        ImGui::SetCursorPosX(windowWidth - buttonWidth);
        bool ret = ImGui::Button(resetlabel);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        return ret;
    }

    void VisualsMenu::RenderSun()
    {
        bool modified = false;

        if (DrawResettableSectionHeader(ICON_FA_SUN "  Sun"))
        {
            visuals.sunBrightness = defaultVisuals.sunBrightness;
            visuals.sunColorUI = defaultVisuals.sunColorUI;
            visuals.sunDirectionUI = defaultVisuals.sunDirectionUI;
            visuals.sunPitch = defaultVisuals.sunPitch;
            visuals.sunYaw = defaultVisuals.sunYaw;
            modified = true;
        }

        modified = ImGuiEx::Keyframeable::ColorEdit3("Color", glm::value_ptr(visuals.sunColorUI),
                                                     Types::KeyframeablePropertyType::SunLightColor) ||
                   modified;

        modified = ImGuiEx::Keyframeable::SliderFloat("Brightness", &visuals.sunBrightness, 0, 4,
                                                      Types::KeyframeablePropertyType::SunLightBrightness) ||
                   modified;

        ImGui::Dummy(ImVec2(0.0f, 20.0f));  // Spacing

        // ImGui::SliderAngle sets sun angles in radians but displays them as degrees
        modified = ImGuiEx::Keyframeable::SliderAngle("Pitch", &visuals.sunPitch, 0, 360,
                                                      Types::KeyframeablePropertyType::SunLightPitch) ||
                   modified;

        modified = ImGuiEx::Keyframeable::SliderAngle("Yaw", &visuals.sunYaw, 0, 360,
                                                      Types::KeyframeablePropertyType::SunLightYaw) ||
                   modified;

        if (modified)
        {
            UpdateSunAngle();
        }

        ImGui::Separator();
    }

    void VisualsMenu::UpdateDof()
    {
        Mod::GetGameInterface()->SetDof(visuals.dof);
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
        Mod::GetGameInterface()->SetFilmtweaks(visuals.filmtweaks);
    }

    void VisualsMenu::UpdateHudInfo()
    {
        // Do not update HUD settings when using any free-floating camera
        // We do not want to overwrite cg_draw2D for example
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        if (camera->IsModControlledCameraMode())
            return;

        Mod::GetGameInterface()->SetHudInfo(visuals.hudInfo);
    }

    void VisualsMenu::LoadPreset(Preset preset)
    {
        auto newConfiguration = Components::VisualConfiguration::Load(preset.path);
        if (newConfiguration.has_value())
        {
            visuals = newConfiguration.value();

            UpdateDof();
            UpdateSun();
            SetAngleFromPosition(visuals.sunDirectionUI);
            UpdateFilmtweaks();
            UpdateHudInfo();

            AddPresetToRecent(preset);
            currentPreset = preset;
        }
    }

    void VisualsMenu::AddPresetToRecent(Preset newPreset)
    {
        for (size_t i = 0; i < recentPresets.size(); ++i)
        {
            Preset preset = recentPresets[i];
            if (preset.path == newPreset.path)
            {
                recentPresets.erase(recentPresets.begin() + i);
                break;
            }
        }
        recentPresets.push_back(newPreset);
    }

    void VisualsMenu::Release()
    {
    }
}  // namespace IWXMVM::UI