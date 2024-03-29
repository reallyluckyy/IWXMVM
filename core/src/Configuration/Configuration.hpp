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
        void Write(bool quiet = false);

        template <typename T>
        static void ReadValueInto(nlohmann::json json, std::string_view node, T& out)
        {
            try
            {
                out = json.at(node).get<T>();
            }
            catch (std::exception)
            {
            }
        }

       private:
        Configuration()
        {
        }

        std::filesystem::path GetUserConfigPath();

        const std::string_view NODE_KEYBINDS = "keybinds";
        const std::string_view NODE_PREFERENCES = "preferences";
    };
}  // namespace IWXMVM