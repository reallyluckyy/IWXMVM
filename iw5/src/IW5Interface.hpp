#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Events.hpp"
#include "Patches.hpp"
#include "Functions.hpp"
#include "Hooks.hpp"
#include "Hooks/HUD.hpp"
#include "Hooks/Camera.hpp"
#include "Components/Rewinding.hpp"
#include "Utilities/PathUtils.hpp"

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
            Hooks::Install();
            Patches::GetGamePatches();
        }

        void RemovePlutoniumCallbacks()
        {
            HWND hwnd = D3D9::FindWindowHandle();
            DWORD wndprocAddr = GetWindowLongPtr(hwnd, GWLP_WNDPROC);

            // mw3 plutonium has a wndproc callback hook that we need to remove
            if (wndprocAddr != Mod::GetGameInterface()->GetWndProc())
            {
                // remove plutonium's wndproc hook and restore it with the game's original wndproc
                SetWindowLongPtr(hwnd, GWL_WNDPROC, Mod::GetGameInterface()->GetWndProc());

                // remove plutonium's raw input device hook
                RAWINPUTDEVICE hid;
                hid.usUsagePage = 1;  // HID_USAGE_PAGE_GENERIC
                hid.usUsage = 2;      // HID_MOUSE
                hid.dwFlags = RIDEV_REMOVE;
                hid.hwndTarget = NULL;
                RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
            }
        }

        void SetupEventListeners() final
        {
            RemovePlutoniumCallbacks();

            Events::RegisterListener(EventType::OnCameraChanged, []() {
                auto& camera = Components::CameraManager::Get().GetActiveCamera();
                auto isFreeCamera = camera->IsModControlledCameraMode();

                auto demoPlayback = Structures::GetClientDemoPlayback();
                demoPlayback->cameraMode = isFreeCamera ? 2 : 0;
                demoPlayback->cameraModeChanged = 1;

                Functions::FindDvar("cg_draw2d")->current.boolean = isFreeCamera ? 0 : 1;
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
            
            if (Structures::GetClientConnection()->clientDemo.state == 2)
                return Types::GameState::InDemo;
            
            return Types::GameState::InGame;
        }

        Types::Features GetSupportedFeatures() final
        {
            return Types::Features_None;
        }

        void InitializeGameAddresses() final
        {
            char mainModuleName[MAX_PATH];
            GetModuleFileName(NULL, mainModuleName, MAX_PATH);

            bool isRunningPluto =
                std::string(mainModuleName).find("plutonium-bootstrapper-win32.exe") != std::string::npos;

            LOG_DEBUG("Using Plutonium: {}", isRunningPluto);

            try
            {
                GetGameAddresses();
            }
            catch (std::exception& ex)
            {
                LOG_ERROR("Failed to find required signature: {}", ex.what());
                MessageBoxA(NULL,
                            "It seems like you are running an unsupported game version.\nYou can download a "
                            "supported version of MW3 at the following link:\n\n"
                            "https://codmvm.com/data/iwxmvm/iw5mp.exe\n\nSimply replace the iw5mp.exe in your MW3 "
                            "directory with this one.",
                            "Unsupported game version", MB_OK);
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

        std::filesystem::path GetDemoDirectory()
        {
            return std::filesystem::path(GetDvar("fs_basepath")->value->string) / "main" / "demo";
        }

        Types::DemoInfo GetDemoInfo() final
        {
            static uint32_t demoStartTick, demoEndTick;
            Functions::CL_Demo_GetStartAndEndTime(&demoStartTick, &demoEndTick);

            Types::DemoInfo demoInfo;
            demoInfo.name = Structures::GetClientStatic()->servername;
            demoInfo.name = demoInfo.name.starts_with(DEMO_TEMP_DIRECTORY)
                                ? demoInfo.name.substr(strlen(DEMO_TEMP_DIRECTORY) + 1)
                                : demoInfo.name;

            demoInfo.path = (GetDemoDirectory() / demoInfo.name).string();

            demoInfo.endTick = demoEndTick - demoStartTick;
            demoInfo.gameTick = Structures::GetClientGlobals()->time - demoStartTick;

            return demoInfo;
        }

        std::string_view GetDemoExtension() final
        {
            return {".demo"};
        }

        void PlayDemo(std::filesystem::path demoPath) final
        {
            Events::Invoke(EventType::PreDemoLoad);
            
            const auto demoDirectory = GetDemoDirectory();

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
                static float znear = 1.0f;
                return Types::Dvar{name, (Types::Dvar::Value*)&znear};
            }

             // TODO: Hack #2. I would, once again, strongly advise against exposing dvars directly to core...
            if (name.compare("timescale") == 0)
            {
                // in plutonium clientdemoplayback timescale is not initialized until you watch a demo
                if (Structures::GetClientDemoPlayback() == nullptr)
                    return std::nullopt;

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
            Hooks::Camera::firstPersonFOV = fov;
        }

        Types::Sun GetSun() final
        {
            const auto cg = Structures::GetClientGlobals();
            for (auto& light : std::span{cg->refdef.primaryLights})
            {
                if (light.type == Structures::GfxLightType::GFX_LIGHT_TYPE_DIR)
                {
                    Types::Sun sunType = {
                        glm::make_vec3(light.color),
                        glm::degrees(glm::make_vec3(light.dir)),
                        1.0f
                    };
                    return sunType;
                }
            }

            return {};
        }

        Types::DoF GetDof()
        {
            Types::DoF dof = {
                static_cast<bool>(Functions::FindDvar("r_dof_tweak")->current.boolean) &&
                    static_cast<bool>(Functions::FindDvar("r_dof_enable")->current.boolean),
                Functions::FindDvar("r_dof_farBlur")->current.value,
                Functions::FindDvar("r_dof_farStart")->current.value,
                Functions::FindDvar("r_dof_farEnd")->current.value,
                Functions::FindDvar("r_dof_nearBlur")->current.value,
                Functions::FindDvar("r_dof_nearStart")->current.value,
                Functions::FindDvar("r_dof_nearEnd")->current.value,
                Functions::FindDvar("r_dof_bias")->current.value
            };

            return dof;
        }

        Types::Filmtweaks GetFilmtweaks()
        {
            Types::Filmtweaks filmtweaks = {
                static_cast<bool> (Functions::FindDvar("r_filmUseTweaks")->current.boolean) &&
                    static_cast<bool> (Functions::FindDvar("r_filmTweakEnable")->current.boolean),
                Functions::FindDvar("r_filmTweakBrightness")->current.value,
                Functions::FindDvar("r_filmTweakContrast")->current.value,
                Functions::FindDvar("r_filmTweakDesaturation")->current.value,
                glm::make_vec3(Functions::FindDvar("r_filmTweakLightTint")->current.vec3),
                glm::make_vec3(Functions::FindDvar("r_filmTweakDarkTint")->current.vec3),
                static_cast<bool>(Functions::FindDvar("r_filmTweakInvert")->current.boolean)
            };

            return filmtweaks;
        }

        Types::HudInfo GetHudInfo()
        {
            Types::HudInfo hudInfo = {
                Functions::FindDvar("cg_draw2D")->current.boolean,
                !Functions::FindDvar("ui_hud_hardcore")->current.boolean,
                Hooks::HUD::showShellshockEffects,
                Functions::FindDvar("ui_drawCrosshair")->current.boolean,
                Hooks::HUD::showScore,
                Hooks::HUD::showIconsAndText,
                Hooks::HUD::showBloodOverlay,
                Functions::FindDvar("ui_hud_obituaries")->current.string[0] == '1',
                glm::vec3(
                    Functions::FindDvar("cg_TeamColor_Allies")->current.color[0] / 255.0f,
                    Functions::FindDvar("cg_TeamColor_Allies")->current.color[1] / 255.0f,
                    Functions::FindDvar("cg_TeamColor_Allies")->current.color[2] / 255.0f
                ),
                glm::vec3(
                    Functions::FindDvar("cg_TeamColor_Axis")->current.color[0] / 255.0f,
                    Functions::FindDvar("cg_TeamColor_Axis")->current.color[1] / 255.0f,
                    Functions::FindDvar("cg_TeamColor_Axis")->current.color[2] / 255.0f
                ),
            };

            return hudInfo;
        }

        void SetSun(Types::Sun sun) final
        {
            const auto cg = Structures::GetClientGlobals();
            for (auto& light : std::span{cg->refdef.primaryLights})
            {
                if (light.type == Structures::GfxLightType::GFX_LIGHT_TYPE_DIR)
                {
                    light.color[0] = sun.color.x;
                    light.color[1] = sun.color.y;
                    light.color[2] = sun.color.z;
                    light.dir[0] = glm::radians(sun.direction.x);
                    light.dir[1] = glm::radians(sun.direction.y);
                    light.dir[2] = glm::radians(sun.direction.z);
                }
            }
        }

        void SetDof(Types::DoF dof) final
        {
            Functions::FindDvar("r_dof_tweak")->current.boolean = dof.enabled;
            Functions::FindDvar("r_dof_enable")->current.boolean = dof.enabled;

            Functions::FindDvar("r_dof_farBlur")->current.value = dof.farBlur;
            Functions::FindDvar("r_dof_farStart")->current.value = dof.farStart;
            Functions::FindDvar("r_dof_farEnd")->current.value = dof.farEnd;

            // hacky workaround from the IW3 implementation because i think it carries over
            if (dof.nearBlur < 1.3f)
            {
                dof.nearBlur = 5;
                dof.nearStart = 0;
                dof.nearEnd = 0;
            }

            Functions::FindDvar("r_dof_nearBlur")->current.value = dof.nearBlur;
            Functions::FindDvar("r_dof_nearStart")->current.value = dof.nearStart;
            Functions::FindDvar("r_dof_nearEnd")->current.value = dof.nearEnd;

            Functions::FindDvar("r_dof_bias")->current.value = dof.bias;
        }

        void SetFilmtweaks(Types::Filmtweaks filmtweaks) final
        {
            Functions::FindDvar("r_filmUseTweaks")->current.boolean = filmtweaks.enabled;
            Functions::FindDvar("r_filmTweakEnable")->current.boolean = filmtweaks.enabled;
            Functions::FindDvar("r_filmTweakBrightness")->current.value = filmtweaks.brightness;
            Functions::FindDvar("r_filmTweakContrast")->current.value = filmtweaks.contrast;
            Functions::FindDvar("r_filmTweakDesaturation")->current.value = filmtweaks.desaturation;
            for (int i = 0; i < 3; ++i)
            {
                Functions::FindDvar("r_filmTweakLightTint")->current.vec3[i] =
                    glm::value_ptr(filmtweaks.tintLight)[i];
                Functions::FindDvar("r_filmTweakDarkTint")->current.vec3[i] = glm::value_ptr(filmtweaks.tintDark)[i];
            }
            Functions::FindDvar("r_filmTweakInvert")->current.boolean = filmtweaks.invert;
        }

        void SetHudInfo(Types::HudInfo hudInfo) final
        {
            Functions::FindDvar("con_gamemsgwindow0msgtime")->current.value = 5;
            Functions::FindDvar("con_gamemsgwindow0linecount")->current.integer = 4;

            Functions::FindDvar("cg_draw2D")->current.boolean = hudInfo.show2DElements;

            Functions::FindDvar("g_hardcore")->current.boolean = !hudInfo.showPlayerHUD;
            Functions::FindDvar("scr_gameended")->current.string = hudInfo.showPlayerHUD ? "0" : "1";
            Functions::FindDvar("cg_centertime")->current.value = hudInfo.showPlayerHUD ? 5.0f : 0.0f;
            Functions::FindDvar("cg_overheadranksize")->current.value = hudInfo.showPlayerHUD ? 0.5f : 0;
            Functions::FindDvar("cg_overheadnamessize")->current.value = hudInfo.showPlayerHUD ? 0.5f : 0;
            Functions::FindDvar("cg_overheadiconsize")->current.value = hudInfo.showPlayerHUD ? 0.7f : 0;
            Functions::FindDvar("waypointiconwidth")->current.value = hudInfo.showPlayerHUD ? 32.0f : 0.001f;
            Functions::FindDvar("waypointoffscreenpointerwidth")->current.value = hudInfo.showPlayerHUD ? 32 : 0.001f;

            Hooks::HUD::showShellshockEffects = hudInfo.showShellshock;
            Functions::FindDvar("ui_drawCrosshair")->current.boolean = hudInfo.showCrosshair;
            Hooks::HUD::showScore = hudInfo.showScore;

            Hooks::HUD::showIconsAndText = hudInfo.showIconsAndText;
            Hooks::HUD::showBloodOverlay = hudInfo.showBloodOverlay;
            if (hudInfo.showBloodOverlay)
                Patches::GetGamePatches().CG_PainVisionUpdate.Revert();
			else
                Patches::GetGamePatches().CG_PainVisionUpdate.Apply();

            if (hudInfo.showKillfeed)
                Patches::GetGamePatches().Item_GameMsgWindow_Paint.Revert();
            else
                Patches::GetGamePatches().Item_GameMsgWindow_Paint.Apply();

            if (hudInfo.showIconsAndText)
                Patches::GetGamePatches().CG_DrawRotatedPic.Revert();
            else
                Patches::GetGamePatches().CG_DrawRotatedPic.Apply();

            for (int i = 0; i < 3; i++)
                Functions::FindDvar("cg_TeamColor_Allies")->current.color[i] = static_cast<uint8_t>(hudInfo.killfeedTeam1Color[i] * 255.0f);
            for (int i = 0; i < 3; i++)
                Functions::FindDvar("cg_TeamColor_Axis")->current.color[i] = static_cast<uint8_t>(hudInfo.killfeedTeam2Color[i] * 255.0f);
        }
        
        std::vector<Types::Entity> GetEntities() final
        {
            std::vector<Types::Entity> entities;

            auto cg_entities = Structures::GetEntities();

            auto ToEntityType = [](char eType) -> Types::EntityType {
                switch (eType)
                {
                    case Structures::entityType_t::ET_PLAYER:
                        return Types::EntityType::Player;
                    case Structures::entityType_t::ET_PLAYER_CORPSE:
                        return Types::EntityType::Corpse;
                    case Structures::entityType_t::ET_ITEM:
                        return Types::EntityType::Item;
                    case Structures::entityType_t::ET_MISSILE:
                        return Types::EntityType::Missile;
                    case Structures::entityType_t::ET_HELICOPTER:
                        return Types::EntityType::Helicopter;
                    default:
                        return Types::EntityType::Unsupported;
                }
            };

            for (int i = 0; i < 256; i++)
            {
                auto entity = cg_entities[i];
                entities.push_back(
                    Types::Entity{
                        .id = i,
                        .type = ToEntityType(entity.pose.eType),
                        .clientNum = entity.nextState.clientNum,
                        .isValid = (bool)(entity.flags & 1),
                    }
                );
            }

            return entities;
        }

        auto FindBoneIndex(Structures::DObj* dobj, uint16_t boneName)
        {
            if (!dobj->models || !dobj->numModels)
                return -1;

            auto boneIndex = -1;

            auto totalBones = 0;
            for (int m = 0; m < dobj->numModels; m++)
            {
                auto model = dobj->models[m];
                if (!model || !model->numBones)
                    return -1;
                for (int b = 0; b < model->numBones; b++)
                {
                    auto bone = model->boneNames[b];
                    if (bone == boneName)
                    {
                        boneIndex = totalBones + b;
                    }
                }
                totalBones += model->numBones;
            }

            return boneIndex;
        }

        Types::BoneData GetBoneData(int32_t entityId, const std::string& name) final
        {
            Structures::DObj* dobj = Functions::Com_GetClientDObj(entityId);

            if (!dobj)
            {
                return {.id = -1};
            }

            auto entities = Structures::GetEntities();
            auto entity = &entities[entityId];
            auto boneName = Functions::Scr_AllocString(name.c_str());

            const auto orgTimeStamp = std::exchange(dobj->skel.timeStamp, Structures::GetClientStatic()->skelTimeStamp);

            auto boneIndex = FindBoneIndex(dobj, boneName);
            if (boneIndex == -1)
            {
                dobj->skel.timeStamp = orgTimeStamp;
                return {.id = -1};
            }

            float rotationMatrix[3 * 3];
            float origin[3];
            auto result = Functions::CG_DObjGetWorldBoneMatrix(entity, boneIndex, (float*)rotationMatrix, dobj, origin);

            dobj->skel.timeStamp = orgTimeStamp;
            if (!result)
            {
                LOG_ERROR("Call to CG_DObjGetWorldBoneMatrix failed");
                return {.id = -1};
            }

            Types::BoneData boneData;
            boneData.id = boneIndex;
            boneData.position = glm::make_vec3(origin);
            boneData.rotation = glm::make_mat3(rotationMatrix);
            return boneData;
        }

        constexpr std::vector<std::string> GetSupportedBoneNames()
        {
            return
            {
                "tag_weapon", 
                "tag_weapon_left", 
                "tag_weapon_right",
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
            uintptr_t CL_FirstSnapshot = GetGameAddresses().CL_FirstSnapshot();

            Patches::GetGamePatches().Con_TimeJumped.Apply();

            _asm
            {
                pushad
                xor esi, esi
                call CL_FirstSnapshot
                popad
             }

            Patches::GetGamePatches().Con_TimeJumped.Revert();
        }

        void ResetClientData(int serverTime)
        {
            auto cl = Structures::GetClientActive();
            for (auto& snapshot : std::span{cl->snapshots})
                snapshot.valid = 0;

            cl->snap.serverTime = serverTime;
            cl->serverTime = 0;
            cl->oldFrameServerTime = 0;
            cl->serverTimeDelta = 0;

            auto clc = Structures::GetClientConnection();
            //clc->timeDemoFrames = 0;
            //clc->timeDemoStart = 0;
            //clc->timeDemoPrev = 0;
            //clc->timeDemoBaseTime = 0;

            auto cls = Structures::GetClientStatic();
            cls->realtime = 0;
            //cls->realFrametime = 0;

            auto cgs = Structures::GetClientGlobalsStatic();
            cgs->processedSnapshotNum = 0;

            auto cg = Structures::GetClientGlobals();
            cg->latestSnapshotNum = 0;
            cg->latestSnapshotTime = 0;
            cg->snap = 0;
            cg->nextSnap = 0;
            cg->landTime = 0;
        }

        Types::PlaybackData GetPlaybackDataAddresses() const
        {
            auto cl = Structures::GetClientActive();
            auto clc = Structures::GetClientConnection();
            auto cgs = Structures::GetClientGlobalsStatic();
            auto cls = Structures::GetClientStatic();

            return Types::PlaybackData{
                .cl =
                {
                    .snap_serverTime = reinterpret_cast<uintptr_t>(&cl->snap.serverTime),
                    .serverTime = reinterpret_cast<uintptr_t>(&cl->serverTime),
                    .parseEntitiesNum = reinterpret_cast<uintptr_t>(&cl->parseEntitiesIndex),
                    .parseClientsNum = reinterpret_cast<uintptr_t>(&cl->parseClientsIndex),
                },
                .clc = 
                {
                    .serverCommandSequence = reinterpret_cast < uintptr_t > (&clc->serverCommandSequence),
                    .lastExecutedServerCommand = reinterpret_cast<uintptr_t>(&clc->lastExecutedServerCommand),
                        .serverCommands = {.address = reinterpret_cast<uintptr_t>(&clc->serverCommands),
                                        .size = 128 * 1024},
                    .serverConfigDataSequence = 0
                },
                .cgs =
                {
                    .serverCommandSequence = reinterpret_cast<uintptr_t>(&cgs->serverCommandSequence),
                },
                .cls =
                {
                    .realtime = reinterpret_cast<uintptr_t>(&cls->realtime) - 4,
                },
                .s_compassActors = {.address = GetGameAddresses().s_compassActors(),
                                    .size = 18 * sizeof(Structures::CompassActor)},
                //.teamChatMsgs = 
                //{
                //    .address = reinterpret_cast<uintptr_t>(Structures::GetClientGlobalsStatic()->teamChatMsgs),
                //    .size = 8 * 160 + 4 * 8 + 4 + 4
                //},
                .cg_entities = 
                {
                    .address = reinterpret_cast<uintptr_t>(Structures::GetEntities()), 
                    .size = 0x800 * sizeof(Structures::centity_s)
                },
                .clientInfo = 
                {
                    .address = reinterpret_cast<uintptr_t>(Structures::GetClientInfo()),
                    .size = 18 * sizeof(Structures::clientInfo_t)
                },
                .gameState = 
                {
                    .address = reinterpret_cast<uintptr_t>(Structures::GetGameState()),
                    .size = sizeof(Structures::gameState_t)
                },
                //.killfeed = GetGameAddresses().conGameMsgWindow0()
            };
        }

        void ExecuteNewServerCommands() final
        {
            const auto clc = Structures::GetClientConnection();
            const auto cgs = Structures::GetClientGlobalsStatic();

            const auto oldServerCommandSequence = cgs->serverCommandSequence;
            const auto newServerCommandSequence = clc->serverCommandSequence;

            static constexpr auto dvar = 'd';
            for (auto i = oldServerCommandSequence + 1; i <= newServerCommandSequence; ++i)
            {
                if (clc->serverCommands[i & 127][0] != dvar)
                {
                    continue;
                }

                const std::string_view cmd{clc->serverCommands[i & 127]};
                if (cmd.find(R"("mpintro")") != std::string_view::npos ||
                    cmd.find(R"("mpIntro")") != std::string_view::npos)
                {
                    // erase server commands that set black and white vision
                    clc->serverCommands[i & 127][0] = '\0';
                }
            }

            // check if the command string backlog is equal to or greater than half the size of command string buffer
            // (128 / 2 = 64)
            if (oldServerCommandSequence > 0 &&
                oldServerCommandSequence + std::ssize(clc->serverCommands) / 2 <= newServerCommandSequence)
            {
                for (auto i = oldServerCommandSequence + 1; i <= newServerCommandSequence; ++i)
                {
                    if (clc->serverCommands[i & 127][0] != dvar)
                    {
                        // erase server commands that do not modify the gamestate strings
                        clc->serverCommands[i & 127][0] = '\0';
                    }
                }

                typedef int(__cdecl * CG_ExecuteNewServerCommands_t)(int localClientNum, int newServerCommandSequence);
                CG_ExecuteNewServerCommands_t CG_ExecuteNewServerCommands = (CG_ExecuteNewServerCommands_t)GetGameAddresses().CG_ExecuteNewServerCommands();
                CG_ExecuteNewServerCommands(0, newServerCommandSequence);

                for (auto i = oldServerCommandSequence + 1; i <= newServerCommandSequence; ++i)
                {
                    // erase server commands to prevent double processing; shouldn't be necessary but just to be sure
                    clc->serverCommands[i & 127][0] = '\0';
                }
            }
        }

        
        void HideScoreboard() final
        {
            typedef void (*CG_ScoresUp_t)(int localClientNum);
            CG_ScoresUp_t CG_ScoresUp = (CG_ScoresUp_t)GetGameAddresses().CG_ScoresUp();

            CG_ScoresUp(0);

            // We need to set this to 1 in case we were in a match ending killcam
            // before the rewind happened
            float* com_codeTimescale = (float*)GetGameAddresses().com_codeTimeScale();
            *com_codeTimescale = 1.0f;
        }

        // IW5 demos have both a header and a footer that contain some metadata.
        // We need to account for these when reading the demo file ourselves to determine when the
        // end of the demo file is reached.

        uint32_t GetDemoFooterSize() final
        {
            return 596;
        }

        uint32_t GetDemoHeaderSize() final
        {
            return 12320;
        }
    };
}  // namespace IWXMVM::IW5
