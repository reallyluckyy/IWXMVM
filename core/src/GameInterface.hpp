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

namespace IWXMVM
{
	const auto DEMO_TEMP_DIRECTORY = "IWXTMP";

	class GameInterface
	{
	public:

		GameInterface(const Types::Game game) : game(game) {}
		virtual ~GameInterface() = default;

		Types::Game GetGame() const { return game; }

		virtual void InstallHooksAndPatches() = 0;
		virtual void SetupEventListeners() = 0;

		virtual uintptr_t GetWndProc() = 0;
		virtual void SetMouseMode(Types::MouseMode mode) = 0;
		virtual Types::GameState GetGameState() = 0;

		virtual std::optional<std::span<HMODULE>> GetModuleHandles(Types::ModuleType type = Types::ModuleType::BaseModule)
		{
			assert(type != Types::ModuleType::SecondaryModules);

			static std::vector<HMODULE> modules{ ::GetModuleHandle(nullptr) };
			return std::span{ modules };
		}
		virtual void InitializeGameAddresses() = 0;

		virtual Types::DemoInfo GetDemoInfo() = 0;
		virtual std::string_view GetDemoExtension() = 0;

		virtual void PlayDemo(std::filesystem::path demoPath) = 0;
		virtual void Disconnect() = 0;

		virtual void ToggleDemoPlaybackState() = 0;
		virtual bool IsDemoPlaybackPaused() = 0;

		virtual std::atomic<std::int32_t>& GetTickDelta() = 0;
		virtual void SetTickDelta(std::int32_t) = 0;

		// perhaps dvars shouldnt be exposed to core at all?
		virtual std::optional<Types::Dvar> GetDvar(const std::string_view name) = 0;

		virtual Types::Sun GetSun() = 0;
		virtual Types::DoF GetDof() = 0;
		virtual void SetSun(Types::Sun) = 0;
		virtual void SetDof(Types::DoF) = 0;

	private:
		Types::Game game;
	};
}
