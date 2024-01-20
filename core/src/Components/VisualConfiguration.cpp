#include "StdInclude.hpp"
#include "VisualConfiguration.hpp"

#include <unordered_set>

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
        "r_lighttweaksunlight"
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
        out << "r_dof_enable " << settings.dofActive << "\n";
        out << "r_dof_farblur " << settings.dofFarBlur << "\n";
        out << "r_dof_farstart " << settings.dofFarStart << "\n";
        out << "r_dof_farend " << settings.dofFarEnd << "\n";
        out << "r_dof_nearblur " << settings.dofNearBlur << "\n";
        out << "r_dof_nearstart " << settings.dofNearStart << "\n";
        out << "r_dof_nearend " << settings.dofNearEnd << "\n";

        // SUN
        out << "r_lighttweaksuncolor " << settings.sunColorUI.r << " " << settings.sunColorUI.g << " "
            << settings.sunColorUI.b << " 1\n";
        out << "r_lighttweaksundirection " << settings.sunDirectionUI.x << " " << settings.sunDirectionUI.y << " "
            << settings.sunDirectionUI.z << "\n";
        out << "r_lighttweaksunlight " << settings.sunBrightness << "\n";

        // FILMTWEAKS
        out << "r_filmtweakenable " << settings.filmtweaksActive << "\n";
        out << "r_filmtweakbrightness " << settings.filmtweaksBrightness << "\n";
        out << "r_filmtweakcontrast " << settings.filmtweaksContrast << "\n";
        out << "r_filmtweakdesaturation " << settings.filmtweaksDesaturation << "\n";
        out << "r_filmtweaklighttint " << settings.filmtweaksTintLight.r << " " << settings.filmtweaksTintLight.g << " "
            << settings.filmtweaksTintLight.b << "\n";
        out << "r_filmtweakdarktint " << settings.filmtweaksTintDark.r << " " << settings.filmtweaksTintDark.g << " "
            << settings.filmtweaksTintDark.b << "\n";
        out << "r_filmtweakinvert " << settings.filmtweaksInvert << "\n";
        out.close();
    }
}