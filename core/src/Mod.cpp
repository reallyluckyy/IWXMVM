#include "StdInclude.hpp"
#include "Mod.hpp"

#include "GameInterface.hpp"
#include "Version.hpp"
#include "WindowsConsole.hpp"
#include "Utilities/HookManager.hpp"
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

			//MemoryUtils::UnprotectModule();

			LOG_DEBUG("Installing game hooks...");
			gameInterface->HookD3D();
			gameInterface->InstallHooks();
			gameInterface->SetupEventListeners();

			// TODO: Initialize Components/Patches
			// TODO: ...

			LOG_INFO("Initialized IWXMVM!");

			while (!UI::UIManager::ejectRequested.load())
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			UI::UIManager::ShutdownImGui();
			LOG_DEBUG("ImGui successfully shutdown");
			HookManager::Unhook();
			LOG_DEBUG("Unhooked");
			SetWindowLongPtr(Mod::GetGameInterface()->GetWindowHandle(), GWLP_WNDPROC, (LONG_PTR)UI::UIManager::GameWndProc);

			WindowsConsole::Close();
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