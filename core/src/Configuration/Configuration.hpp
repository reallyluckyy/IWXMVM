#pragma once
#include "nlohmann/json.hpp"
#include "Input.hpp"

namespace IWXMVM
{
    class Configuration
    {
       public:
        static Configuration& Get()
        {
            static Configuration instance;
            return instance;
        }

        Configuration(Configuration const&) = delete;
        void operator=(Configuration const&) = delete;

        void Initialize();
        void Write();

       private:
        Configuration()
        {
        }

        std::filesystem::path GetUserConfigPath();
        void WriteDefaultConfig();

        const int CONFIG_VERSION = 2;

        const std::string_view NODE_VERSION = "version";
        const std::string_view NODE_KEYBINDS = "keybinds";
    };
}  // namespace IWXMVM