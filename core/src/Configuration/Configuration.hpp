#pragma once
#include "nlohmann/json.hpp"

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

        const std::string_view NODE_KEYBINDS = "keybinds";
        const std::string_view NODE_PREFERENCES = "preferences";
    };
}  // namespace IWXMVM