#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Functions.hpp"
#include "Hooks.hpp"
#include "Events.hpp"
#include "DemoParser.hpp"
#include "Hooks/Camera.hpp"
#include "Hooks/Playback.hpp"
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
                                     []() { Functions::FindDvar("sv_cheats")->current.enabled = true; });
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
            if (!Functions::FindDvar("cl_ingame")->current.enabled)
                return Types::GameState::MainMenu;

            if (Structures::GetClientConnection()->demoplaying)
                return Types::GameState::InDemo;

            return Types::GameState::InGame;
        }

        Types::DemoInfo GetDemoInfo() final
        {
            static uint32_t lastValidTick = 0;

            Types::DemoInfo demoInfo;
            demoInfo.name = Structures::GetClientStatic()->servername;
            demoInfo.name = demoInfo.name.starts_with(DEMO_TEMP_DIRECTORY)
                                ? demoInfo.name.substr(strlen(DEMO_TEMP_DIRECTORY) + 1)
                                : demoInfo.name;

            std::string str = static_cast<std::string>(Structures::GetClientStatic()->servername);
            str += (str.ends_with(".dm_1")) ? "" : ".dm_1";
            demoInfo.path = Functions::GetFilePath(std::move(str));

            demoInfo.isRewinding = IW3::Hooks::Playback::rewindTo.load() != -1;
            if (!demoInfo.isRewinding)
                lastValidTick = Structures::GetClientActive()->serverTime - DemoParser::demoStartTick;
            demoInfo.currentTick = lastValidTick;
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

                Functions::Cbuf_AddText(
                    std::format(R"(demo "{0}/{1}")", DEMO_TEMP_DIRECTORY, targetPath.filename().string()));
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
            const auto iw3Dvar = Functions::FindDvar(name);

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
            Types::DoF dof = 
            {
                Functions::FindDvar("r_dof_tweak")->current.enabled &&
                    Functions::FindDvar("r_dof_enable")->current.enabled,
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
                Functions::FindDvar("r_filmUseTweaks")->current.enabled &&
                    Functions::FindDvar("r_filmTweakEnable")->current.enabled,
                Functions::FindDvar("r_filmTweakBrightness")->current.value,
                Functions::FindDvar("r_filmTweakContrast")->current.value,
                Functions::FindDvar("r_filmTweakDesaturation")->current.value,
                glm::make_vec3(Functions::FindDvar("r_filmTweakLightTint")->current.vector),
                glm::make_vec3(Functions::FindDvar("r_filmTweakDarkTint")->current.vector),
                Functions::FindDvar("r_filmTweakInvert")->current.enabled
            };

            return filmtweaks;
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
            Functions::FindDvar("r_dof_tweak")->current.enabled = dof.enabled;
            Functions::FindDvar("r_dof_enable")->current.enabled = dof.enabled;
            Functions::FindDvar("r_dof_farBlur")->current.value = dof.farBlur;
            Functions::FindDvar("r_dof_farStart")->current.value = dof.farStart;
            Functions::FindDvar("r_dof_farEnd")->current.value = dof.farEnd;
            Functions::FindDvar("r_dof_nearBlur")->current.value = dof.nearBlur;
            Functions::FindDvar("r_dof_nearStart")->current.value = dof.nearStart;
            Functions::FindDvar("r_dof_nearEnd")->current.value = dof.nearEnd;
            Functions::FindDvar("r_dof_bias")->current.value = dof.bias;
        }

        void SetFilmtweaks(Types::Filmtweaks filmtweaks) final
        {
            Functions::FindDvar("r_filmUseTweaks")->current.enabled = filmtweaks.enabled;
            Functions::FindDvar("r_filmTweakEnable")->current.enabled = filmtweaks.enabled;
            Functions::FindDvar("r_filmTweakBrightness")->current.value = filmtweaks.brightness;
            Functions::FindDvar("r_filmTweakContrast")->current.value = filmtweaks.contrast;
            Functions::FindDvar("r_filmTweakDesaturation")->current.value = filmtweaks.desaturation;
            for (int i = 0; i < 3; ++i)
            {
                Functions::FindDvar("r_filmTweakLightTint")->current.vector[i] =
                    glm::value_ptr(filmtweaks.tintLight)[i];
                Functions::FindDvar("r_filmTweakDarkTint")->current.vector[i] = glm::value_ptr(filmtweaks.tintDark)[i];
            }
            Functions::FindDvar("r_filmTweakInvert")->current.enabled = filmtweaks.invert;
        }

        void SetTickDelta(std::int32_t value) final
        {
            if (value > 0)
                Hooks::Playback::SkipForward(value);
            else if (value < -300)
                Hooks::Playback::RewindBy(value);
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
                auto pose = (Structures::cpose_t*)(&cg_entities[i]);
                if (pose->eType)
                {
                    entities.push_back(
                        Types::Entity
                        {
                            .id = i, 
                            .type = ToEntityType(pose->eType)
                        }
                    );
                }
            }

            return entities;
		}

        auto FindBoneIndex(Structures::DObj_s* dobj, uint16_t boneName)
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
            uint16_t* clientObjMap = Structures::GetClientObjectMap();
            Structures::DObj_s* objBuf = Structures::GetObjBuf();

            uint16_t dobjIndex = clientObjMap[entityId];
            Structures::DObj_s* dobj = &objBuf[dobjIndex];

            auto entities = Structures::GetEntities();
            auto entity = &entities[entityId];
            auto boneName = Functions::SL_GetStringOfSize(name.c_str(), 1, name.size() + 1);

            auto boneIndex = FindBoneIndex(dobj, boneName);
            if (boneIndex == -1)
            {
                //LOG_ERROR("Bone {0} was not found in {1} models", boneName, (int)dobj->numModels);
                return {.id = -1};
            }

            float rotationMatrix[3 * 3];
            float origin[3];
            auto result =
                Functions::CG_DObjGetWorldBoneMatrix(entity, boneIndex, (float*)rotationMatrix, dobj, origin);

            if (!result)
            {
                LOG_ERROR("Call to CG_DObjGetWorldTagMatrix failed");
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
                "tag_flash",     
                "tag_clip",      
                "tag_brass",
                "j_head",
                "j_wrist_le",
                "j_wrist_ri",
                "j_shoulder_le",
                "j_shoulder_ri",
                "j_ankle_le",
                "j_ankle_ri"
            };
        }
    };
}  // namespace IWXMVM::IW3
