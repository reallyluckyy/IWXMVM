#include "StdInclude.hpp"
#include "Mod.hpp"

#include "GameInterface.hpp"
#include "Version.hpp"
#include "WindowsConsole.hpp"
#include "Utilities/PathUtils.hpp"
#include "Utilities/MemoryUtils.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM
{
	GameInterface* Mod::internalGameInterface = nullptr;

	void Mod::Initialize(GameInterface* gameInterface)
	{
		try 
		{
			internalGameInterface = gameInterface;

			WindowsConsole::Open();
			Logger::Initialize();

			LOG_INFO("Loading IWXMVM {}", IWXMVM_VERSION);
			LOG_INFO("Game: {}", gameInterface->GetGameName());
			LOG_INFO("Game Path: {}", PathUtils::GetCurrentExecutablePath());

			MemoryUtils::UnprotectModule();

			LOG_DEBUG("Installing game hooks...");
			gameInterface->InstallHooks();
			gameInterface->SetupEventListeners();

			UI::UIManager::Initialize();
			// TODO: Initialize Components/Patches
			// TODO: ...

			LOG_INFO("Initialized IWXMVM!");
		}
		catch (std::exception& ex)
		{
			LOG_ERROR("An error occurred during initialization: {}", ex.what());
			// TODO: What do we do here?
		}
		catch (...)
		{
			LOG_ERROR("An error occurred during initialization");
		}
	}
}
