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

            validDvars = {"r_dof_enable",
                          "r_dof_farblur",
                          "r_dof_farstart",
                          "r_dof_farend",
                          "r_dof_nearblur",
                          "r_dof_nearstart",
                          "r_dof_nearEnd",
                          "r_filmtweakenable",
                          "r_filmtweakbrightness",
                          "r_filmtweakcontrast",
                          "r_filmtweakdesaturation",
                          "r_filmtweaklighttint",
                          "r_filmtweakdarktint",
                          "r_filmtweakinvert",       
                          "r_lighttweaksuncolor", 
                          "r_lighttweaksundirection",
                          "r_lighttweaksunlight"
            };

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
        CHAR szFile[1024] = {0}; // TODO: Do something about this.

        OPENFILENAMEA ofn = {};
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
            LOG_ERROR("Failed to load config file: {}", preset.path.string());
            return;
        }
        
        std::string dvar;
        std::string strValue;
        
        
        while (!in.eof() || tokenBacklog != "")
        {
            dvar = GetNextToken(in);

            if (validDvars.find(dvar) == validDvars.end())
                continue;

            strValue = GetNextToken(in);
            float value;
            if (!ConvertStringToFloat(strValue, value))
                continue;

            // DOF
            if (dvar == "r_dof_enable")
                visuals.dofActive = value;
            else if (dvar == "r_dof_farblur")
                visuals.dofFarBlur = value;
            else if (dvar == "r_dof_farstart")
                visuals.dofFarStart = value;
            else if (dvar == "r_dof_farend")
                visuals.dofFarEnd = value;
            else if (dvar == "r_dof_nearblur")
                visuals.dofNearBlur = value;
            else if (dvar == "r_dof_nearstart")
                visuals.dofNearStart = value;
            else if (dvar == "r_dof_nearend")
                visuals.dofNearEnd = value;

            // SUN
            else if (dvar == "r_lighttweaksuncolor")
            {
                std::string sg, sb, sa;
                float r = value;
                float g, b, a;
                sg = GetNextToken(in);
                if (!ConvertStringToFloat(sg, g))
                    continue;
                sb = GetNextToken(in);
                if (!ConvertStringToFloat(sb, b))
                    continue;
                sa = GetNextToken(in);
                if (!ConvertStringToFloat(sa, a))
                    continue;
                visuals.sunColorUI = glm::vec3(r, g, b);
            }
            else if (dvar == "r_lighttweaksundirection")
            {
                std::string sy, sz;
                float x = value;
                float y, z;
                sy = GetNextToken(in);
                if (!ConvertStringToFloat(sy, y))
                    continue;
                sz = GetNextToken(in);
                if (!ConvertStringToFloat(sz, z))
                    continue;
                visuals.sunDirectionUI = glm::vec3(x, y, z);
            }
            else if (dvar == "r_lighttweaksunlight")
                visuals.sunBrightness = value;

            // FILMTWEAKS
            else if (dvar == "r_filmtweakenable")
                visuals.filmtweaksActive = value;
            else if (dvar == "r_filmtweakbrightness")
                visuals.filmtweaksBrightness = value;
            else if (dvar == "r_filmtweakcontrast")
                visuals.filmtweaksContrast = value;
            else if (dvar == "r_filmtweakdesaturation")
                visuals.filmtweaksDesaturation = value;
            else if (dvar == "r_filmtweaklighttint")
            {
                std::string sg, sb;
                float r = value;
                float g, b;
                sg = GetNextToken(in);
                if (!ConvertStringToFloat(sg, g))
                    continue;
                sb = GetNextToken(in);
                if (!ConvertStringToFloat(sb, b))
                    continue;
                visuals.filmtweaksTintLight = glm::vec3(r, g, b);
            }
            else if (dvar == "r_filmtweakdarktint")
            {
                std::string sg, sb;
                float r = value;
                float g, b;
                sg = GetNextToken(in);
                if (!ConvertStringToFloat(sg, g))
                    continue;
                sb = GetNextToken(in);
                if (!ConvertStringToFloat(sb, b))
                    continue;
                visuals.filmtweaksTintDark = glm::vec3(r, g, b);
            }
            else if (dvar == "r_filmtweakinvert")
                visuals.filmtweaksInvert = value;
        }

        UpdateDof();
        UpdateSun();
        SetAngleFromPosition(visuals.sunDirectionUI);
        UpdateFilmtweaks();

        AddPresetToRecent(preset);
        currentPreset = preset;
    }

    std::string VisualsMenu::GetNextToken(std::ifstream& in)
    {
        if (tokenBacklog != "")
        {
            std::string token = tokenBacklog;
            tokenBacklog = "";
            if (token != "set" && token != "seta" && token != "sets" && token != "setu")
                return token;
        }
        while (!in.eof())
        {
            std::string token;
            in >> token;

            if (ProcessString(token))
                return token;
        }
        return "";
    }

    bool VisualsMenu::ProcessString(std::string& str)
    {
        str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return c == '\"'; }),
                  str.end());  // remove quotes

        std::string lowerStr{};
        for (char c : str)
            lowerStr += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (lowerStr == "set" || lowerStr == "seta" || lowerStr == "sets")
            return false;

        // at most, the string stores `value;nextdvar` without whitespace
        int semicolonPos = lowerStr.find(';');
        if (semicolonPos != std::string::npos)
        {
            str = lowerStr.substr(0, semicolonPos);
            tokenBacklog = lowerStr.substr(semicolonPos + 1);
            return true;
        }

        str = lowerStr;
        return true;
    }

    bool VisualsMenu::ConvertStringToFloat(std::string& str, float& val)
    {
        try
        {
            val = std::stof(str); // is there a method like this for string_view ?
            return true;
        }
        catch (const std::invalid_argument&)
        {
            return false;
        }
        catch (const std::out_of_range&)
        {
            return false;
        }
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