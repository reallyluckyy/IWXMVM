#include "StdInclude.hpp"
#include "Mod.hpp"

#include "WindowsConsole.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM
{
	Game Mod::currentGame;

	void Mod::Initialize(const Game game)
	{
		currentGame = game;

		WindowsConsole::Open();
		Logger::Initialize();

		LOG_INFO("Loading IWXMVM {}", "version goes here");
		LOG_INFO("Game ID: {}", (uint32_t)currentGame);
		LOG_INFO("Game Path: {}", PathUtils::GetCurrentExecutablePath());

		// TODO: Initialize Components/Patches
		// TODO: Initialize UI
		// TODO: ...

		LOG_INFO("Initialized IWXMVM!");
	}
}
