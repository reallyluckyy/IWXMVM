#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Hooks.hpp"
#include "Events.hpp"
#include "DemoParser.hpp"
#include "Hooks/Camera.hpp"
#include "Addresses.hpp"
#include "Patches.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace IWXMVM::IW3
{
    class IW3Interface : public GameInterface
    {
       public:
        IW3Interface() : GameInterface(Types::Game::IW3)
        {
        }

        void InstallHooksAndPatches() final
        {
            Hooks::Install();
            Patches::GetGamePatches();
        }

        void SetupEventListeners() final
        {
            Events::RegisterListener(EventType::OnDemoLoad, DemoParser::Run);

            Events::RegisterListener(EventType::OnCameraChanged, Hooks::Camera::OnCameraChanged);

            Events::RegisterListener(EventType::OnDemoLoad,
                                     []() { Structures::FindDvar("sv_cheats")->current.enabled = true; });
        }

        uintptr_t GetWndProc() final
        {
            return (uintptr_t)GetGameAddresses().MainWndProc();
        }

        void SetMouseMode(Types::MouseMode mode) final
        {
            Structures::GetMouseVars()->mouseInitialized = (mode == Types::MouseMode::Capture) ? false : true;
        }

        Types::GameState GetGameState() final
        {
            if (!Structures::FindDvar("cl_ingame")->current.enabled)
                return Types::GameState::MainMenu;

            if (Structures::GetClientConnection()->demoplaying)
                return Types::GameState::InDemo;

            return Types::GameState::InGame;
        }

        // TODO: cache this
        Types::DemoInfo GetDemoInfo() final
        {
            Types::DemoInfo demoInfo;
            demoInfo.name = Structures::GetClientStatic()->servername;
            demoInfo.name = demoInfo.name.starts_with(DEMO_TEMP_DIRECTORY)
                                ? demoInfo.name.substr(strlen(DEMO_TEMP_DIRECTORY) + 1)
                                : demoInfo.name;

            std::string str = static_cast<std::string>(Structures::GetClientStatic()->servername);
            str += (str.ends_with(".dm_1")) ? "" : ".dm_1";
            demoInfo.path = Structures::GetFilePath(std::move(str));

            demoInfo.currentTick = Structures::GetClientActive()->serverTime - DemoParser::demoStartTick;
            demoInfo.endTick = DemoParser::demoEndTick - DemoParser::demoStartTick;

            return demoInfo;
        }

        std::string_view GetDemoExtension() final
        {
            return {".dm_1"};
        }

        void PlayDemo(std::filesystem::path demoPath) final
        {
            const auto demoDirectory =
                std::filesystem::path(GetDvar("fs_basepath")->value->string) / "players" / "demos";

            try
            {
                LOG_INFO("Playing demo {0}", demoPath.string());

                if (!std::filesystem::exists(demoPath) || !std::filesystem::is_regular_file(demoPath))
                    return;

                const auto tempDemoDirectory = demoDirectory / DEMO_TEMP_DIRECTORY;
                if (!std::filesystem::exists(tempDemoDirectory))
                    std::filesystem::create_directories(tempDemoDirectory);

                const auto targetPath = tempDemoDirectory / demoPath.filename();
                if (std::filesystem::exists(targetPath) && std::filesystem::is_regular_file(targetPath))
                    std::filesystem::remove(targetPath);

                std::filesystem::copy(demoPath, targetPath);

                Structures::Cbuf_AddText(
                    std::format(R"(demo "{0}/{1}")", DEMO_TEMP_DIRECTORY, targetPath.filename().string()));
            }
            catch (std::filesystem::filesystem_error& e)
            {
                LOG_ERROR("Failed to play demo file {0}: {1}", demoPath.string(), e.what());
            }
        }

        void Disconnect()
        {
            Structures::Cbuf_AddText("disconnect");
        }

        bool isPlaybackPaused = false;

        void ToggleDemoPlaybackState() final
        {
            isPlaybackPaused = !isPlaybackPaused;
        }

        bool IsDemoPlaybackPaused() final
        {
            return isPlaybackPaused;
        }

        bool IsConsoleOpen() final
        {
            return (Structures::GetClientUIActives()->keyCatchers & 1) != 0;
		}

        void InitializeGameAddresses() final
        {
            GetGameAddresses();
        }

        static HMODULE GetCoD4xModuleHandle()
        {
            std::string moduleName = "cod4x_021.dll";
            MODULEINFO moduleData{};

            for (std::size_t i = 0; i < 1000; ++i)
            {
                if (!::GetModuleInformation(::GetCurrentProcess(), ::GetModuleHandle(moduleName.c_str()), &moduleData,
                                            sizeof(moduleData)) ||
                    moduleData.lpBaseOfDll)
                {
                    break;
                }

                moduleName = "cod4x_" + std::format("{:03}", i) + ".dll";
            }

            return static_cast<HMODULE>(moduleData.lpBaseOfDll);
        }

        std::optional<std::span<HMODULE>> GetModuleHandles(Types::ModuleType type = Types::ModuleType::BaseModule) final
        {
            static std::vector<HMODULE> modules = []() {
                if (const HMODULE moduleCoD4X = GetCoD4xModuleHandle(); moduleCoD4X != 0)
                    return std::vector<HMODULE>{::GetModuleHandle(nullptr), moduleCoD4X};
                else
                    return std::vector<HMODULE>{::GetModuleHandle(nullptr)};
            }();

            assert(modules.size() >= 1);

            if (type == Types::ModuleType::BaseModule)
                return std::span{modules.begin(), modules.begin() + 1};
            else
            {
                if (modules.size() > 1)
                    return std::span{modules.begin() + 1, modules.end()};
                else
                    return std::nullopt;
            }
        }

        std::optional<Types::Dvar> GetDvar(const std::string_view name) final
        {
            const auto iw3Dvar = Structures::FindDvar(name);

            if (!iw3Dvar)
                return std::nullopt;

            Types::Dvar dvar;
            dvar.name = iw3Dvar->name;
            dvar.value = (Types::Dvar::Value*)&iw3Dvar->current;

            return dvar;
        }

        Types::Sun GetSun() final
        {
            auto gfxWorld = Structures::GetGfxWorld();

            Types::Sun sun;
            sun.color = glm::make_vec3(gfxWorld->sunLight->color);
            sun.direction = glm::make_vec3(gfxWorld->sunLight->dir);
            sun.brightness = 1;
            return sun;
        }

        Types::DoF GetDof()
        {
            Types::DoF dof = {Structures::FindDvar("r_dof_tweak")->current.enabled &&
                                  Structures::FindDvar("r_dof_enable")->current.enabled,
                              Structures::FindDvar("r_dof_farBlur")->current.value,
                              Structures::FindDvar("r_dof_farStart")->current.value,
                              Structures::FindDvar("r_dof_farEnd")->current.value,
                              Structures::FindDvar("r_dof_nearBlur")->current.value,
                              Structures::FindDvar("r_dof_nearStart")->current.value,
                              Structures::FindDvar("r_dof_nearEnd")->current.value,
                              Structures::FindDvar("r_dof_bias")->current.value};

            return dof;
        }

        void SetSun(Types::Sun sun) final
        {
            auto gfxWorld = Structures::GetGfxWorld();

            for (int i = 0; i < 3; ++i)
            {
                gfxWorld->sunLight->color[i] = glm::value_ptr(sun.color)[i] * sun.brightness;
                gfxWorld->sunLight->dir[i] = glm::value_ptr(sun.direction)[i];
            }
        }

        void SetDof(Types::DoF dof) final
        {
            Structures::FindDvar("r_dof_tweak")->current.enabled = dof.enabled;
            Structures::FindDvar("r_dof_enable")->current.enabled = dof.enabled;
            Structures::FindDvar("r_dof_farBlur")->current.value = dof.farBlur;
            Structures::FindDvar("r_dof_farStart")->current.value = dof.farStart;
            Structures::FindDvar("r_dof_farEnd")->current.value = dof.farEnd;
            Structures::FindDvar("r_dof_nearBlur")->current.value = dof.nearBlur;
            Structures::FindDvar("r_dof_nearStart")->current.value = dof.nearStart;
            Structures::FindDvar("r_dof_nearEnd")->current.value = dof.nearEnd;
            Structures::FindDvar("r_dof_bias")->current.value = dof.bias;
        }

        std::atomic<std::int32_t> tick;

        std::atomic<std::int32_t>& GetTickDelta() final
        {
            return tick;
        }

        void SetTickDelta(std::int32_t value) final
        {
            if (value > 0)
                Structures::GetClientStatic()->realtime += value;
            else if (value < 0)
                tick.store(value);
        }
    };
}  // namespace IWXMVM::IW3
