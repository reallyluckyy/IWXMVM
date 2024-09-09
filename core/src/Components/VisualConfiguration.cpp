#include "StdInclude.hpp"
#include "VisualConfiguration.hpp"

#include <unordered_set>

// TODO: rework this somehow
namespace IWXMVM::Components
{
    std::string tokenBacklog;

    std::unordered_set<std::string> validDvars = 
    {
        "r_dof_enable",          
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
        "r_lighttweaksunlight",
        "cg_draw2d",
        "ui_hud_hardcore",
        "cg_drawshellshock",
        "ui_drawcrosshair",
        "iwxmvm_ui_showscore",
        "iwxmvm_ui_showothertext",
        "iwxmvm_ui_showbloodoverlay",
        "ui_hud_obituaries",
        "g_teamcolor_allies",
        "g_teamcolor_axis"
    };

    bool ProcessString(std::string& str)
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

    std::string GetNextToken(std::ifstream& in)
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

    bool ConvertStringToFloat(std::string& str, float& val)
    {
        try
        {
            val = std::stof(str);  // is there a method like this for string_view ?
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

    std::optional<VisualConfiguration::Settings> VisualConfiguration::Load(std::filesystem::path file)
    {
        VisualConfiguration::Settings visuals;

        std::ifstream in(file);
        if (!in.is_open())
        {
            LOG_ERROR("Failed to load config file: {}", file.string());
            return std::nullopt;
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
                visuals.dof.enabled = value;
            else if (dvar == "r_dof_farblur")
                visuals.dof.farBlur = value;
            else if (dvar == "r_dof_farstart")
                visuals.dof.farStart = value;
            else if (dvar == "r_dof_farend")
                visuals.dof.farEnd = value;
            else if (dvar == "r_dof_nearblur")
                visuals.dof.nearBlur = value;
            else if (dvar == "r_dof_nearstart")
                visuals.dof.nearStart = value;
            else if (dvar == "r_dof_nearend")
                visuals.dof.nearEnd = value;

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
                visuals.sunColor = glm::vec3(r, g, b);
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
                visuals.sunDirection = glm::vec3(x, y, z);
            }
            else if (dvar == "r_lighttweaksunlight")
                visuals.sunBrightness = value;

            // FILMTWEAKS
            else if (dvar == "r_filmtweakenable")
                visuals.filmtweaks.enabled = value;
            else if (dvar == "r_filmtweakbrightness")
                visuals.filmtweaks.brightness = value;
            else if (dvar == "r_filmtweakcontrast")
                visuals.filmtweaks.contrast = value;
            else if (dvar == "r_filmtweakdesaturation")
                visuals.filmtweaks.desaturation = value;
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
                visuals.filmtweaks.tintLight = glm::vec3(r, g, b);
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
                visuals.filmtweaks.tintDark = glm::vec3(r, g, b);
            }
            else if (dvar == "r_filmtweakinvert")
                visuals.filmtweaks.invert = value;

            // MISC
            else if (dvar == "cg_draw2d")
                visuals.hudInfo.show2DElements = value;
            else if (dvar == "ui_hud_hardcore")
                visuals.hudInfo.showPlayerHUD = !value;
            else if (dvar == "cg_drawshellshock")
                visuals.hudInfo.showShellshock = value;
            else if (dvar == "ui_drawcrosshair")
                visuals.hudInfo.showCrosshair = value;
            else if (dvar == "iwxmvm_ui_showscore")
                visuals.hudInfo.showScore = value;
            else if (dvar == "ui_hud_obituaries")
                visuals.hudInfo.showKillfeed = value;
            else if (dvar == "g_teamcolor_allies")
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
                visuals.hudInfo.killfeedTeam1Color = glm::vec3(r, g, b);
            }
            else if (dvar == "g_teamcolor_axis")
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
                visuals.hudInfo.killfeedTeam2Color = glm::vec3(r, g, b);
            }
        }

        return visuals;
    }

    void VisualConfiguration::Save(std::filesystem::path file, VisualConfiguration::Settings settings)
    {
        std::ofstream out(file);
        if (!out.is_open())
        {
            LOG_ERROR("Failed to save config file: {}", file.string());
            return;
        }

        // DOF
        out << "r_dof_enable " << settings.dof.enabled << "\n";
        out << "r_dof_farblur " << settings.dof.farBlur << "\n";
        out << "r_dof_farstart " << settings.dof.farStart << "\n";
        out << "r_dof_farend " << settings.dof.farEnd << "\n";
        out << "r_dof_nearblur " << settings.dof.nearBlur << "\n";
        out << "r_dof_nearstart " << settings.dof.nearStart << "\n";
        out << "r_dof_nearend " << settings.dof.nearEnd << "\n";

        // SUN
        out << "r_lighttweaksuncolor " << settings.sunColor.r << " " << settings.sunColor.g << " "
            << settings.sunColor.b << " 1\n";
        out << "r_lighttweaksundirection " << settings.sunDirection.x << " " << settings.sunDirection.y << " "
            << settings.sunDirection.z << "\n";
        out << "r_lighttweaksunlight " << settings.sunBrightness << "\n";

        // FILMTWEAKS
        out << "r_filmtweakenable " << settings.filmtweaks.enabled << "\n";
        out << "r_filmtweakbrightness " << settings.filmtweaks.brightness << "\n";
        out << "r_filmtweakcontrast " << settings.filmtweaks.contrast << "\n";
        out << "r_filmtweakdesaturation " << settings.filmtweaks.desaturation << "\n";
        out << "r_filmtweaklighttint " << settings.filmtweaks.tintLight.r << " " << settings.filmtweaks.tintLight.g
            << " " << settings.filmtweaks.tintLight.b << "\n";
        out << "r_filmtweakdarktint " << settings.filmtweaks.tintDark.r << " " << settings.filmtweaks.tintDark.g << " "
            << settings.filmtweaks.tintDark.b << "\n";
        out << "r_filmtweakinvert " << settings.filmtweaks.invert << "\n";

        // MISC
        out << "cg_draw2d " << settings.hudInfo.show2DElements << "\n";
        out << "ui_hud_hardcore " << !settings.hudInfo.showPlayerHUD << "\n";
        out << "cg_drawshellshock " << settings.hudInfo.showShellshock << "\n";
        out << "ui_drawcrosshair " << settings.hudInfo.showCrosshair << "\n";
        out << "iwxmvm_ui_showscore " << settings.hudInfo.showScore << "\n";
        out << "iwxmvm_ui_showothertext " << settings.hudInfo.showIconsAndText << "\n";
        out << "iwxmvm_ui_showbloodoverlay " << settings.hudInfo.showBloodOverlay << "\n";
        out << "ui_hud_obituaries " << settings.hudInfo.showKillfeed << "\n";
        out << "g_teamcolor_allies " << settings.hudInfo.killfeedTeam1Color[0] << " "
            << settings.hudInfo.killfeedTeam1Color[1] << " " << settings.hudInfo.killfeedTeam1Color[2]
            << " 1\n";
        out << "g_teamcolor_axis " << settings.hudInfo.killfeedTeam2Color[0] << " "
            << settings.hudInfo.killfeedTeam2Color[1] << " " << settings.hudInfo.killfeedTeam2Color[2]
            << " 1\n";
        out.close();
    }
}