#pragma once
#include "StdInclude.hpp"

#include "D3D9Helper.hpp"
#include "Components/Camera.hpp"
#include "Types/GameState.hpp"
#include "Types/Game.hpp"
#include "Types/DemoInfo.hpp"
#include "Types/MouseMode.hpp"
#include "Types/Dvar.hpp"

namespace IWXMVM
{
	const auto DEMO_TEMP_DIRECTORY = "IWXTMP";

	class GameInterface
	{
	public:

		GameInterface(const Types::Game game) : game(game) {}
		virtual ~GameInterface() = default;

		Types::Game GetGame() const { return game; }

		HWND GetWindowHandle() const { return D3D9Helper::FindWindowHandle(); }
		IDirect3DDevice9* GetD3D9Device() const { return D3D9Helper::GetDevicePtr(); }

		void HookD3D() const { D3D9Helper::Initialize(); }
		virtual void InstallHooks() = 0;
		virtual void SetupEventListeners() = 0;

		virtual uintptr_t GetWndProc() = 0;
		virtual void SetMouseMode(Types::MouseMode mode) = 0;
		virtual Types::GameState GetGameState() = 0;

		virtual Types::DemoInfo GetDemoInfo() = 0;
		virtual std::string_view GetDemoExtension() = 0;

		virtual void PlayDemo(std::filesystem::path demoPath) = 0;

		virtual void ToggleDemoPlaybackState() = 0;
		virtual bool IsDemoPlaybackPaused() = 0;

		// perhaps dvars shouldnt be exposed to core at all?
		virtual std::optional<Types::Dvar> GetDvar(const std::string name) = 0;

	private:
		Types::Game game;
	};
}
