#include "StdInclude.hpp"
#include "Configuration.hpp"

#include "Utilities/PathUtils.hpp"
#include "InputConfiguration.hpp"

namespace IWXMVM
{

	void Configuration::Initialize()
	{
		std::ifstream configFile(GetUserConfigPath());
		if (!configFile.is_open()) 
		{
			WriteDefaultConfig();
			configFile.open(GetUserConfigPath());
		}

		nlohmann::json config = nlohmann::json::parse(configFile);
		configFile.close();

		auto configVersion = config.at(NODE_VERSION).get<int32_t>();
		if (configVersion != CONFIG_VERSION)
		{
			LOG_WARN("Config version mismatch (expected: {}, actual: {})", CONFIG_VERSION, configVersion);
			LOG_WARN("Please consider deleting your config. Some keys may not be bound!");
			// TODO: what do we do here?
		}

		InputConfiguration::Get().Deserialize(config[NODE_KEYBINDS]);
	}

	void Configuration::Write()
	{
		using json = nlohmann::json;

		json config;
		config[NODE_VERSION] = CONFIG_VERSION;

		InputConfiguration::Get().Serialize(config[NODE_KEYBINDS]);

		auto configPath = GetUserConfigPath();
		if (!std::filesystem::exists(configPath.parent_path()))
			std::filesystem::create_directories(configPath.parent_path());

		std::ofstream configFile(configPath);
		configFile << config.dump(4);
		configFile.close();
	}

	std::filesystem::path Configuration::GetUserConfigPath()
	{
		return std::filesystem::path(PathUtils::GetCurrentGameDirectory()) / "IWXMVM" / "config.json";
	}

	void Configuration::WriteDefaultConfig()
	{
		InputConfiguration::Get().LoadDefault();
		Write();
	}
}