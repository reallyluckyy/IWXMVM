#include "StdInclude.hpp"
#include "Mod.hpp"

#include "GameInterface.hpp"
#include "Version.hpp"
#include "WindowsConsole.hpp"
#include "Input.hpp"
#include "Utilities/HookManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Utilities/MemoryUtils.hpp"
#include "UI/UIManager.hpp"
#include "Configuration/Configuration.hpp"

namespace IWXMVM
{
    GameInterface* Mod::internalGameInterface = nullptr;
    std::atomic<bool> Mod::ejectRequested = false;

    HMODULE GetCurrentModule()
    {
        static bool dummy;

        MEMORY_BASIC_INFORMATION mbi;
        ::VirtualQuery(&dummy, &mbi, sizeof(mbi));

        return static_cast<HMODULE>(mbi.AllocationBase);
    }

    void Mod::RequestEject()
    {
        ejectRequested.store(true);
    }

    void Mod::Initialize(GameInterface* gameInterface)
    {
        try
        {
            internalGameInterface = gameInterface;

            WindowsConsole::Open();
            Logger::Initialize();

            LOG_INFO("Loading IWXMVM {}", IWXMVM_VERSION);
            LOG_INFO("Game: {}", magic_enum::enum_name(gameInterface->GetGame()));
            LOG_INFO("Game Path: {}", PathUtils::GetCurrentExecutablePath());

            LOG_DEBUG("Scanning signatures...");
            gameInterface->InitializeGameAddresses();

            LOG_DEBUG("Installing game hooks and patches...");
            D3D9::Initialize();
            gameInterface->InstallHooksAndPatches();
            gameInterface->SetupEventListeners();

            LOG_DEBUG("Initializing components...");
            Configuration::Get().Initialize();
            Components::CameraManager::Get().Initialize();
            Components::CampathManager::Get().Initialize();
            Components::KeyframeManager::Get().Initialize();
            Components::CaptureManager::Get().Initialize();
            Components::Rewinding::Initialize();

            LOG_INFO("Initialized IWXMVM!");

            while (!ejectRequested.load())
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            HookManager::Unhook();
            LOG_DEBUG("Unhooked");
            UI::UIManager::Get().ShutdownImGui();
            LOG_DEBUG("ImGui successfully shutdown");

            WindowsConsole::Close();
            ::FreeLibraryAndExitThread(GetCurrentModule(), 0);
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
}  // namespace IWXMVM