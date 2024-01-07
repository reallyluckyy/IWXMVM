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
        // TODO: Ideally we dont want to ever overwrite a users custom settings, but what if the map changes

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
            recentPresets = {};

            SetAngleFromPosition(sun.direction);

            defaultVisuals = visuals;
            currentPreset = {"Default"};
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
        if (ImGui::BeginCombo("##configCombo", currentPreset.name.c_str()))
        {
            if (ImGui::Selectable(ICON_FA_CHESS_KING " Default Preset", currentPreset.name == "Default"))
            {
                visuals = defaultVisuals;
                UpdateDof();
                UpdateSun();
                SetAngleFromPosition(visuals.sunDirectionUI);
                UpdateFilmtweaks();
                currentPreset = {"Default"};
            }

            ImGui::Separator();
            
            
            for (auto& preset : recentPresets)
            {
                if (ImGui::Selectable((std::string(ICON_FA_ARROW_ROTATE_RIGHT) + " " + preset.name + "##" + preset.path.string()).c_str(),
                                      currentPreset.name == preset.name))
                    LoadConfig(preset);
            }
            
            ImGui::Separator();
            if (ImGui::Selectable(ICON_FA_FOLDER_OPEN " Load from file", false))
            {
                Preset preset = OpenFileDialog();
                if (preset.path != "")
                    LoadConfig(preset);
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

    VisualsMenu::Preset VisualsMenu::OpenFileDialog()
    {
        OPENFILENAMEA ofn;
        CHAR szFile[1024] = {0}; // TODO: Do something about this.

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Config\0*.cfg\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            auto path = std::filesystem::u8path(szFile);
            return {path.filename().string(), path};
        }
        else
            return {};
    }

    void VisualsMenu::LoadConfig(Preset preset)
    {
        std::ifstream in(preset.path);
        if (!in.is_open())
        {
            //LOG_ERROR("Cannot open preset file from {0:x}", path);
            return;
        }
        
        std::string dvar;
        std::string value;
        
        // TODO: Refactor all this
        while (!in.eof())
        {
            in >> dvar;

            dvar.erase(std::remove_if(dvar.begin(), dvar.end(), [](char c) { return c == '\"'; }), dvar.end());
            if (dvar == "seta")
                continue;

            in >> value;

            value.erase(std::remove_if(value.begin(), value.end(), [](char c) { return c == '\"'; }),
                        value.end());  // removes all occurences of quotes

            // TODO: Add error handling for invalid configs.
            // TODO: Make it non case sensitive?
            
            // DOF
            if (dvar == "r_dof_enable")
                visuals.dofActive = std::stof(value);
            else if (dvar == "r_dof_farBlur")
                visuals.dofFarBlur = std::stof(value);
            else if (dvar == "r_dof_farStart")
                visuals.dofFarStart = std::stof(value);
            else if (dvar == "r_dof_farEnd")
                visuals.dofFarEnd = std::stof(value);
            else if (dvar == "r_dof_nearBlur")
                visuals.dofNearBlur = std::stof(value);
            else if (dvar == "r_dof_nearStart")
                visuals.dofNearStart = std::stof(value);
            else if (dvar == "r_dof_nearEnd")
                visuals.dofNearEnd = std::stof(value);

            // SUN
            else if (dvar == "r_lightTweakSunColor")
            {
                std::string g, b, a;
                float r = std::stof(value);
                in >> g;
                in >> b;
                in >> a;
                g.erase(std::remove_if(g.begin(), g.end(), [](char c) { return c == '\"'; }), g.end());
                b.erase(std::remove_if(b.begin(), b.end(), [](char c) { return c == '\"'; }), b.end());
                visuals.sunColorUI = glm::vec3(r, std::stof(g), std::stof(b));
            }
            else if (dvar == "r_lightTweakSunDirection")
            {
                std::string y, z;
                float x = std::stof(value);
                in >> y;
                in >> z;
                y.erase(std::remove_if(y.begin(), y.end(), [](char c) { return c == '\"'; }), y.end());
                z.erase(std::remove_if(z.begin(), z.end(), [](char c) { return c == '\"'; }), z.end());
                visuals.sunDirectionUI = glm::vec3(x, std::stof(y), std::stof(z));
            }
            else if (dvar == "r_lightTweakSunLight")
                visuals.sunBrightness = std::stof(value);

            // FILMTWEAKS
            else if (dvar == "r_filmTweakEnable")
                visuals.filmtweaksActive = std::stof(value);
            else if (dvar == "r_filmTweakBrightness")
                visuals.filmtweaksBrightness = std::stof(value);
            else if (dvar == "r_filmTweakContrast")
                visuals.filmtweaksContrast = std::stof(value);
            else if (dvar == "r_filmTweakDesaturation")
                visuals.filmtweaksDesaturation = std::stof(value);
            else if (dvar == "r_filmTweakLightTint")
            {
                std::string g, b, a;
                float r = std::stof(value);
                in >> g;
                in >> b;
                in >> a;
                g.erase(std::remove_if(g.begin(), g.end(), [](char c) { return c == '\"'; }), g.end());
                b.erase(std::remove_if(b.begin(), b.end(), [](char c) { return c == '\"'; }), b.end());
                visuals.filmtweaksTintLight = glm::vec3(r, std::stof(g), std::stof(b));
            }
            else if (dvar == "r_filmTweakLightDark")
            {
                std::string g, b, a;
                float r = std::stof(value);
                in >> g;
                in >> b;
                in >> a;
                g.erase(std::remove_if(g.begin(), g.end(), [](char c) { return c == '\"'; }), g.end());
                b.erase(std::remove_if(b.begin(), b.end(), [](char c) { return c == '\"'; }), b.end());
                visuals.filmtweaksTintDark = glm::vec3(r, std::stof(g), std::stof(b));
            }
            else if (dvar == "r_filmTweakInvert")
                visuals.filmtweaksInvert = std::stof(value);
        }

        UpdateDof();
        UpdateSun();
        SetAngleFromPosition(visuals.sunDirectionUI);
        UpdateFilmtweaks();

        AddPresetToRecent(preset);
        currentPreset = preset;
    }

    void VisualsMenu::AddPresetToRecent(Preset newPreset)
    {
        for (int i = 0; i < recentPresets.size(); ++i)
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