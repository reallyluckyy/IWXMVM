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

            Hooks::Install();
            Patches::GetGamePatches();
        }

        void SetupEventListeners() final
        {
            Events::RegisterListener(EventType::OnCameraChanged, []() {
                auto& camera = Components::CameraManager::Get().GetActiveCamera();
                auto isFreeCamera = camera->IsModControlledCameraMode();

                auto demoPlayback = Structures::GetClientDemoPlayback();
                demoPlayback->cameraMode = isFreeCamera ? 2 : 0;
                demoPlayback->cameraModeChanged = 1;

                Functions::FindDvar("cg_draw2d")->current.boolean = isFreeCamera ? 0 : 1;
            });

            Events::RegisterListener(EventType::OnFrame, [this]() {
                if (GetGameState() != Types::GameState::InDemo)
                    return;

                auto& camera = Components::CameraManager::Get().GetActiveCamera();
                auto demoCamera = Structures::GetDemoCameraData();
                auto cg = Structures::GetClientGlobals();

                if (!camera->IsModControlledCameraMode())
                {
                    camera->GetPosition() = *reinterpret_cast<glm::vec3*>(cg->refdef.view.org);
                    // TODO: set camera rotation
                    camera->GetFov() = glm::degrees(std::atan(cg->refdef.view.tanHalfFovX) * 2.0f);
                    return;
                }

                demoCamera->demoCameraOrigin[0] = camera->GetPosition()[0];
                demoCamera->demoCameraOrigin[1] = camera->GetPosition()[1];
                demoCamera->demoCameraOrigin[2] = camera->GetPosition()[2];

                demoCamera->demoCameraAngles[0] = camera->GetRotation()[0];
                demoCamera->demoCameraAngles[1] = camera->GetRotation()[1];
                demoCamera->demoCameraAngles[2] = camera->GetRotation()[2];
                
                // TODO: set fov
            });

            Events::RegisterListener(EventType::PostDemoLoad, [&]() { 
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
            static int demoStartTick, demoEndTick;
            Functions::CL_Demo_GetStartAndEndTime(&demoStartTick, &demoEndTick);

            Types::DemoInfo demoInfo;
            demoInfo.name = Structures::GetClientStatic()->servername;
            demoInfo.path = "";

            // I dont really know why the range **isnt** demoStartTick to demoEndTick,
            // but it seems to work this way.
            demoInfo.endTick = demoEndTick;
            demoInfo.currentTick = Structures::GetClientGlobals()->time;

            return demoInfo;
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
            // Theres no console on IW5
            return false;
        }

        std::optional<Types::Dvar> GetDvar(const std::string_view name) final
        {
            // TODO: A hack. Once again, perhaps we shouldnt expose GetDvar to core...
            if (name.compare("r_znear") == 0)
            {
                static float znear = 0.1f;
                return Types::Dvar{name, (Types::Dvar::Value*)&znear};
            }

             // TODO: Hack #2. I would, once again, strongly advise against exposing dvars directly to core...
            if (name.compare("timescale") == 0)
            {
                return Types::Dvar{name, (Types::Dvar::Value*)&Structures::GetClientDemoPlayback()->timeScale};
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
}  // namespace IWXMVM::IW5
