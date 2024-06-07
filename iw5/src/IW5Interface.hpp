#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Events.hpp"
#include "Patches.hpp"
#include "Functions.hpp"
#include "Hooks.hpp"
#include "Components/Rewinding.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace IWXMVM::IW5
{
    class IW5Interface : public GameInterface
    {
       public:
        IW5Interface() : GameInterface(Types::Game::IW5)
        {
        }

        void InstallHooksAndPatches() final
        {
            Structures::clientDemoPlayback_t* playback = new Structures::clientDemoPlayback_t;

            LOG_DEBUG("first {}", (uintptr_t)&playback->localClientNum);
            LOG_DEBUG("cameraMode {}", (uintptr_t)&playback->cameraMode);
            LOG_DEBUG("diff {}", ((uintptr_t)&playback->cameraMode) - (uintptr_t)&playback->localClientNum);


            Hooks::Install();
            Patches::GetGamePatches();
        }

        void SetupEventListeners() final
        {
            //Events::RegisterListener(EventType::PostDemoLoad, DemoParser::Run);
            
            Events::RegisterListener(EventType::OnCameraChanged, []() {
                auto& camera = Components::CameraManager::Get().GetActiveCamera();
                auto isFreeCamera = camera->IsModControlledCameraMode();

                Structures::GetClientDemoPlayback()->cameraMode = isFreeCamera ? 2 : 1;
                Functions::FindDvar("cg_draw2d")->current.boolean = isFreeCamera ? 0 : 1;
            });

            Events::RegisterListener(EventType::OnFrame, []() {
                auto& camera = Components::CameraManager::Get().GetActiveCamera();
                if (!camera->IsModControlledCameraMode())
                {
                    return;
                }

                // TODO: set camera position, rotation, fov
            });

            Events::RegisterListener(EventType::PostDemoLoad, [&]() { 
                Functions::FindDvar("sv_cheats")->current.boolean= true; 
                    
                // ensure these are set to their defaults, so our killfeed toggle works properly
                Functions::FindDvar("con_gamemsgwindow0msgtime")->current.value = 5;
                Functions::FindDvar("con_gamemsgwindow0linecount")->current.integer = 4;
            });
            
        }

        IDirect3DDevice9* GetGameDevicePtr() const final
        {
            return **(IDirect3DDevice9***)GetGameAddresses().d3d9DevicePointer();
        }

        uintptr_t GetWndProc() final
        {
            return (uintptr_t)GetGameAddresses().MainWndProc();
        }

        void SetMouseMode(Types::MouseMode mode) final
        {
            if (mode == Types::MouseMode::Capture)
                Patches::GetGamePatches().IN_Frame.Apply();
            else 
                Patches::GetGamePatches().IN_Frame.Revert();
        }

        Types::GameState GetGameState() final
        {
            if (!Functions::FindDvar("cl_ingame")->current.boolean)
                return Types::GameState::MainMenu;
            
            if (Structures::GetClientConnection()->demoActive)
                return Types::GameState::InDemo;
            
            return Types::GameState::InGame;
        }

        Types::Features GetSupportedFeatures() final
        {
            return Types::Features_None;
        }

        void InitializeGameAddresses() final
        {
            // TODO: check pluto

            //bool isRunningIW3xo = GetModuleHandle("iw3x") != NULL;
            //LOG_DEBUG("Using IW3xo: {}", isRunningIW3xo);

            try
            {
                GetGameAddresses();
            }
            catch (std::exception& ex)
            {
                LOG_ERROR("Failed to find required signature: {}", ex.what());
                //if (isRunningIW3xo)
                //{
                //    MessageBoxA(NULL,
                //                "It seems like you are running an unsupported iw3xo version.\nPlease make sure "
                //                "you are running the latest iw3xo version, which you can find here:\n\n"
                //                "https://github.com/xoxor4d/iw3xo-dev/releases\n\nWe can confirm IWXMVM works with IW3xo 3495!",
                //                "Unsupported iw3xo version", MB_OK);
                //}
                //else
                {
                    MessageBoxA(NULL,
                                "It seems like you are running an unsupported game version.\nYou can download a "
                                "supported version of MW3 at the following link:\n\n"
                                "TODO: insert link here\n\nSimply replace the iw5mp.exe in your MW3 "
                                "directory with this one.",
                                "Unsupported game version", MB_OK);
                }
                ExitProcess(0);
            }
        }

        std::optional<std::span<HMODULE>> GetModuleHandles(Types::ModuleType type = Types::ModuleType::BaseModule) final
        {
            static std::vector<HMODULE> modules = []() {
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

        Types::DemoInfo GetDemoInfo() final
        {
            //static uint32_t lastValidTick = 0;
            //
            //Types::DemoInfo demoInfo;
            //demoInfo.name = Structures::GetClientStatic()->servername;
            //demoInfo.name = demoInfo.name.starts_with(DEMO_TEMP_DIRECTORY)
            //                    ? demoInfo.name.substr(strlen(DEMO_TEMP_DIRECTORY) + 1)
            //                    : demoInfo.name;
            //
            //std::string str = static_cast<std::string>(Structures::GetClientStatic()->servername);
            //str += (str.ends_with(".dm_1")) ? "" : ".dm_1";
            //demoInfo.path = Functions::GetFilePath(std::move(str));
            //
            //auto [demoStartTick, demoEndTick] = DemoParser::GetDemoTickRange();
            //
            //const auto serverTime = Structures::GetClientActive()->serverTime;
            //if (serverTime > demoStartTick && serverTime < demoEndTick && !Components::Rewinding::IsRewinding())
            //    lastValidTick = serverTime - demoStartTick;
            //demoInfo.currentTick = lastValidTick;
            //demoInfo.endTick = demoEndTick - demoStartTick;

            // TODO:

            return {};
        }

        std::string_view GetDemoExtension() final
        {
            return {".demo"};
        }

        void PlayDemo(std::filesystem::path demoPath) final
        {
            Events::Invoke(EventType::PreDemoLoad);
            
            const auto demoDirectory =
                std::filesystem::path(GetDvar("fs_basepath")->value->string) / "main" / "demo";

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

                Functions::Cbuf_AddText(
                    std::format(R"(cl_demo_play "{0}/{1}")", DEMO_TEMP_DIRECTORY, targetPath.filename().string()));
            }
            catch (std::filesystem::filesystem_error& e)
            {
                LOG_ERROR("Failed to play demo file {0}: {1}", demoPath.string(), e.what());
            }
        }

        void Disconnect()
        {
            Functions::Cbuf_AddText("disconnect");
        }

        bool IsConsoleOpen() final
        {
            // TODO: 
            return false;
            //return (Structures::GetClientUIActives()->keyCatchers & 1) != 0;
        }

        std::optional<Types::Dvar> GetDvar(const std::string_view name) final
        {
            // TODO: A hack. Once again, perhaps we shouldnt expose GetDvar to core...
            if (name.compare("r_znear") == 0)
            {
                static float znear = 0.1f;
                return Types::Dvar{name, (Types::Dvar::Value*)&znear};
            }

            const auto gameDvar = Functions::FindDvar(name);

            if (!gameDvar)
                return std::nullopt;

            Types::Dvar dvar;
            dvar.name = gameDvar->name;
            dvar.value = (Types::Dvar::Value*)&gameDvar->current;

            return dvar;
        }

        void SetFov(float fov) final
        {
            LOG_DEBUG("{}", Structures::GetClientDemoPlayback()->cameraMode);
            Functions::FindDvar("cg_fov")->current.value = fov;
        }

        Types::Sun GetSun() final
        {
            // TODO:
            return {};
        }

        Types::DoF GetDof()
        {
            // TODO:
            return {};
        }

        Types::Filmtweaks GetFilmtweaks()
        {
            // TODO:
            return {};
        }

        Types::HudInfo GetHudInfo()
        {
            // TODO:
            return {};
        }

        void SetSun(Types::Sun sun) final
        {
            // TODO:
            return;
        }

        void SetDof(Types::DoF dof) final
        {
            // TODO:
            return;
        }

        void SetFilmtweaks(Types::Filmtweaks filmtweaks) final
        {
            // TODO:
            return;
        }

        void SetHudInfo(Types::HudInfo hudInfo) final
        {
            // TODO:
            return;
        }
        
        std::vector<Types::Entity> GetEntities() final
        {
            // TODO:
            return {};
        }

        Types::BoneData GetBoneData(int32_t entityId, const std::string& name) final
        {
            // TODO:
            return {};
        }

        constexpr std::vector<std::string> GetSupportedBoneNames()
        {
            return
            {
                "tag_weapon", 
                "tag_flash",     
                "tag_clip",      
                "tag_brass",
                "j_head",
                "j_mainroot",
                "j_wrist_le",
                "j_wrist_ri",
                "j_shoulder_le",
                "j_shoulder_ri",
                "j_ankle_le",
                "j_ankle_ri",
                "tag_origin"
            };
        }



        void CL_FirstSnapshot()
        {
            // TODO:
        }

        void ResetClientData(int serverTime)
        {
            // TODO:
        }

        Types::PlaybackData GetPlaybackDataAddresses() const
        {
            // TODO:
            return {};
        }
    };
}  // namespace IWXMVM::IW3
