#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Hooks.hpp"
#include "Events.hpp"
#include "DemoParser.hpp"

#include "Patches/DemoPlaybackPatch.hpp"

namespace IWXMVM::IW3
{
	class IW3Interface : public GameInterface
	{
	public:

		IW3Interface() : GameInterface(Game::IW3) {}

		void InstallHooks() override
		{
			Hooks::Install();
		}

		void SetupEventListeners() override
		{
			Events::RegisterListener(EventType::OnDemoLoad, DemoParser::Run);
		}

		IDirect3DDevice9* GetD3D9Device() override
		{
			return *(IDirect3DDevice9**)0xCC9A408;
		}

		HWND GetWindowHandle() override
		{
			return *(HWND*)0xCC1B6FC;
		}

		uintptr_t GetWndProc() override
		{
			return (uintptr_t)0x57BB20;
		}

		void SetMouseMode(MouseMode mode) override
		{
			Structures::GetMouseVars()->mouseInitialized = mode == MouseMode::Capture ? false : true;
		}

		GameState GetGameState() override
		{
			if (!Structures::FindDvar("cl_ingame")->current.enabled)
				return GameState::MainMenu;

			if (Structures::GetClientConnection()->demoplaying)
				return GameState::InDemo;

			return GameState::InGame;
		}


		// TODO: cache this
		DemoInfo GetDemoInfo() override
		{
			DemoInfo demoInfo;
			demoInfo.name = Structures::GetClientConnection()->demoName;
			demoInfo.path = Structures::GetFilePath(demoInfo.name + ".dm_1");
			demoInfo.currentTick = Structures::GetClientActive()->serverTime - DemoParser::demoStartTick;
			demoInfo.endTick = DemoParser::demoEndTick - DemoParser::demoStartTick;

			return demoInfo;
		}


		void SetDemoPlaybackState(bool paused) override
		{
			if (paused)
				DemoPlaybackPatch::Install();
			else
				DemoPlaybackPatch::Uninstall();
		}

		bool IsDemoPlaybackPaused() override
		{
			return DemoPlaybackPatch::IsInstalled();
		}


		std::optional<Dvar> GetDvar(const std::string name) override
		{
			const auto iw3Dvar = Structures::FindDvar(name);

			if (!iw3Dvar)
				return std::nullopt;

			Dvar dvar;
			dvar.name = iw3Dvar->name;
			dvar.value = (Dvar::Value*)&iw3Dvar->current;

			return dvar;
		}
	};
}
