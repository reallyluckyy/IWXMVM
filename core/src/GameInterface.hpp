#pragma once
#include "StdInclude.hpp"

#include "D3D9.hpp"
#include "Components/Camera.hpp"
#include "Types/GameState.hpp"
#include "Types/Game.hpp"
#include "Types/DemoInfo.hpp"
#include "Types/MouseMode.hpp"
#include "Types/Dvar.hpp"
#include "Types/Sun.hpp"
#include "Types/Dof.hpp"
#include "Types/Filmtweaks.hpp"
#include "Types/BoneData.hpp"
#include "Types/Entity.hpp"
#include "Types/PlaybackData.hpp"
#include "Types/HudInfo.hpp"
#include "Types/Features.hpp"

namespace IWXMVM
{
    const auto DEMO_TEMP_DIRECTORY = "IWXTMP";

    class GameInterface
    {
       public:
        GameInterface(const Types::Game game) : game(game)
        {
        }
        virtual ~GameInterface() = default;

        Types::Game GetGame() const
        {
            return game;
        }

        virtual void InstallHooksAndPatches() = 0;
        virtual void SetupEventListeners() = 0;

        virtual IDirect3DDevice9* GetGameDevicePtr() const = 0;
        virtual uintptr_t GetWndProc() = 0;
        virtual void SetMouseMode(Types::MouseMode mode) = 0;
        virtual Types::GameState GetGameState() = 0;

        virtual std::optional<std::span<HMODULE>> GetModuleHandles(
            Types::ModuleType type = Types::ModuleType::BaseModule)
        {
            assert(type != Types::ModuleType::SecondaryModules);

            static std::vector<HMODULE> modules{::GetModuleHandle(nullptr)};
            return std::span{modules};
        }
        virtual void InitializeGameAddresses() = 0;

        virtual Types::Features GetSupportedFeatures()
        {
            return Types::Features_None;
        };

        virtual std::optional<uint32_t> IsTickFrozen() = 0;
        virtual void ToggleFrozenTick(std::optional<std::uint32_t> newFrozenTick = {}) = 0;
        virtual void UpdateFrozenTick(bool isPaused, std::int32_t gameMsec) = 0;

        virtual Types::DemoInfo GetDemoInfo() = 0;
        virtual std::string_view GetDemoExtension() = 0;

        virtual void PlayDemo(std::filesystem::path demoPath) = 0;
        virtual void Disconnect() = 0;

        virtual bool IsConsoleOpen() = 0;

        // perhaps dvars shouldnt be exposed to core at all?
        virtual std::optional<Types::Dvar> GetDvar(const std::string_view name) = 0;

        virtual void SetFov(float fov) = 0;

        virtual Types::Sun GetSun() = 0;
        virtual Types::DoF GetDof() = 0;
        virtual Types::Filmtweaks GetFilmtweaks() = 0;
        virtual Types::HudInfo GetHudInfo() = 0;
        virtual void SetSun(Types::Sun) = 0;
        virtual void SetDof(Types::DoF) = 0;
        virtual void SetFilmtweaks(Types::Filmtweaks) = 0;
        virtual void SetHudInfo(Types::HudInfo) = 0;

        virtual std::vector<Types::Entity> GetEntities() = 0;
        virtual Types::BoneData GetBoneData(int32_t entityId, const std::string& name) = 0;
        virtual constexpr std::vector<std::string> GetSupportedBoneNames() = 0;

        // == things for rewinding ==
        virtual void CL_FirstSnapshot() = 0;
        virtual void ResetClientData(int serverTime) = 0;
        virtual Types::PlaybackData GetPlaybackDataAddresses() const = 0;

       private:
        Types::Game game;
    };
}  // namespace IWXMVM
