#include "StdInclude.hpp"
#include "Commands.hpp"

#include "../Structures.hpp"
#include "UI/UIManager.hpp"
#include "Events.hpp"

namespace IWXMVM::IW3::Hooks::Commands
{

    struct FunctionStorage
    {
        enum struct CommandType
        {
            Command,
            ServerCommand
        };

        ~FunctionStorage()
        {
            if (hooked)
            {
                Unhook();
            }
        }

        void Hook()
        {
            auto cmd = FindCommand();

            if (cmd == reinterpret_cast<void*>(0xFFFFFFFF))
            {
                return;
            }
            else if (cmd != nullptr && cmd->function != nullptr && *static_cast<uint8_t*>(cmd->function) != 0xC3)
            {
                hooked = true;
                oldFunction = cmd->function;
                cmd->function = newFunction;
            }
            else
            {
                type = (type == CommandType::ServerCommand) ? CommandType::Command : CommandType::ServerCommand;

                cmd = FindCommand();

                if (cmd != nullptr && cmd->function != nullptr && *static_cast<uint8_t*>(cmd->function) != 0xC3)
                {
                    hooked = true;
                    oldFunction = cmd->function;
                    cmd->function = newFunction;
                }
                else
                {
                    LOG_ERROR("An error occurred during hooking the following command: {}", commandName);
                }
            }
        }

        void Unhook()
        {
            auto cmd = FindCommand();

            if (cmd == reinterpret_cast<void*>(0xFFFFFFFF))
            {
                return;
            }
            else if (cmd != nullptr && oldFunction != nullptr)
            {
                hooked = false;
                cmd->function = oldFunction;
            }
            else
            {
                type = (type == CommandType::ServerCommand) ? CommandType::Command : CommandType::ServerCommand;

                cmd = FindCommand();

                if (cmd != nullptr && oldFunction != nullptr)
                {
                    hooked = false;
                    cmd->function = oldFunction;
                }
                else
                {
                    // TODO: this cannot always be called from the destructor (e.g. when the game is closing down)
                    try
                    {
                        LOG_ERROR("An error occurred during unhooking the following command: {}", commandName);
                    }
                    catch (...)
                    {
                    }

                    // provide possibility to look at console output before it's closed
                    Sleep(1000);
                }
            }
        }

        const char* commandName = nullptr;
        CommandType type = CommandType::Command;
        void* newFunction = nullptr;
        void* oldFunction = nullptr;
        bool hooked = false;

       private:
        Structures::cmd_function_t* FindCommand()
        {
            Structures::cmd_function_t** cmd_functions = (Structures::cmd_function_t**)0x1410B3C;
            Structures::cmd_function_t** sv_cmd_functions = (Structures::cmd_function_t**)0x14099DC;

            auto cmd = (type == FunctionStorage::CommandType::Command) ? *cmd_functions : *sv_cmd_functions;

            // structure not found; either wrong address or game's closing down
            if (cmd == nullptr)
                return reinterpret_cast<Structures::cmd_function_t*>(0xFFFFFFFF);

            for (; cmd; cmd = cmd->next)
            {
                if (!strcmp(commandName, cmd->name))
                {
                    if (newFunction != nullptr)
                    {
                        return cmd;
                    }

                    break;
                }
            }

            return nullptr;
        }
    };

    void* FindOriginalFunction(void* hook);

    // TODO: doesn't support fullpath demos [/demo "C:\Path\demo.dm_1" fullpath] yet!
    void CL_PlayDemo_Hook()
    {
        auto* oldFunction = FindOriginalFunction(CL_PlayDemo_Hook);
        if (oldFunction == nullptr)
            return;

        Events::Invoke(EventType::PreDemoLoad);

        reinterpret_cast<void (*)()>(oldFunction)();

        Events::Invoke(EventType::PostDemoLoad);
    }

    std::vector<FunctionStorage> CmdHooks{{"demo", FunctionStorage::CommandType::ServerCommand, CL_PlayDemo_Hook}};

    void* FindOriginalFunction(void* hook)
    {
        for (auto& elem : CmdHooks)
        {
            if (elem.newFunction == hook)
            {
                return elem.oldFunction;
            }
        }

        return nullptr;
    }

    void Install()
    {
        for (auto& elem : CmdHooks)
        {
            elem.Hook();
        }
    }
}  // namespace IWXMVM::IW3::Hooks::Commands
