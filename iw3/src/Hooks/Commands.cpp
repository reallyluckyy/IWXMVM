#include "StdInclude.hpp"
#include "Commands.hpp"

#include "../Structures.hpp"
#include "UI/UIManager.hpp"
#include "Events.hpp"


namespace IWXMVM::IW3::Hooks::Commands 
{

#define MAX_TOKENIZE_STRINGS 8
#define tokenStrings (*(struct CmdArgs*)(0x1410B40))
#define tokenbuf (*(struct CmdArgsPrivate*)(0x1433490))

	struct CmdArgsPrivate
	{
		char textPool[8192];
		const char* argvPool[512];
		int usedTextPool[MAX_TOKENIZE_STRINGS];
		int totalUsedArgvPool;
		int totalUsedTextPool;
	};

	struct CmdArgs
	{
		int nesting;
		int localClientNum[MAX_TOKENIZE_STRINGS];
		int argshift[MAX_TOKENIZE_STRINGS];
		int argc[MAX_TOKENIZE_STRINGS];
		const char** argv[MAX_TOKENIZE_STRINGS];
	};

	int Cmd_Argc()
	{
		return tokenStrings.argc[tokenStrings.nesting];
	}

	const char* Cmd_Argv(int argv)
	{
		if (argv >= Cmd_Argc())
			return "";
		else
			return tokenStrings.argv[tokenStrings.nesting][argv];
	}

	struct FunctionStorage
	{
		enum struct CommandType
		{
			Command,
			ServerCommand
		};

		~FunctionStorage()
		{
			if (hooked) {
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
					try {
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

		reinterpret_cast<void(*)()>(oldFunction)();

		Events::Invoke(EventType::OnDemoLoad);
	}

	// TODO: replayDemo is not supported yet because the old demo path needs to be stored!
	void CL_ReplayDemo_Hook()
	{
		auto* oldFunction = FindOriginalFunction(CL_ReplayDemo_Hook);
		if (oldFunction == nullptr)
			return;

		//reinterpret_cast<void(*)()>(oldFunction)();

		//Events::Invoke(EventType::OnDemoLoad); 
	}

	void CL_Vid_Restart_Hook()
	{
		auto* oldFunction = FindOriginalFunction(CL_Vid_Restart_Hook);
		if (oldFunction == nullptr)
			return;

		if (UI::UIManager::RestartImGui())
			reinterpret_cast<void(*)()>(oldFunction)();
	}

	std::vector<FunctionStorage> CmdHooks{
		{ "vid_restart",	FunctionStorage::CommandType::ServerCommand,	CL_Vid_Restart_Hook },
		{ "demo",		FunctionStorage::CommandType::ServerCommand,	CL_PlayDemo_Hook },
		{ "replayDemo",		FunctionStorage::CommandType::Command,		CL_ReplayDemo_Hook }
	};

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
}