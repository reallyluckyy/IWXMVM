#include "StdInclude.hpp"
#include "Configuration.hpp"

#include "Utilities/PathUtils.hpp"
#include "InputConfiguration.hpp"

namespace IWXMVM
{
    void Configuration::Initialize()
    {
        auto configPath = GetUserConfigPath();
        if (!std::filesystem::exists(configPath.parent_path()))
        {
            std::filesystem::create_directories(configPath.parent_path());
        }

        std::ifstream configFile(configPath);
        if (!configFile.is_open())
        {
            Write();
            configFile.open(configPath);
        }

        try
        {
            nlohmann::json config = nlohmann::json::parse(configFile);
            InputConfiguration::Get().Deserialize(config[NODE_KEYBINDS]);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to parse config file ({}). Falling back to defaults", e.what());
        }

        configFile.close();

        Write();
    }

    void Configuration::Write()
    {
        using json = nlohmann::json;

        json config;

        InputConfiguration::Get().Serialize(config[NODE_KEYBINDS]);

        std::ofstream configFile(GetUserConfigPath());
        configFile << config.dump(4);
        configFile.close();
    }

    std::filesystem::path Configuration::GetUserConfigPath()
    {
        return std::filesystem::path(PathUtils::GetCurrentGameDirectory()) / "IWXMVM" / "config.json";
    }
}  // namespace IWXMVM