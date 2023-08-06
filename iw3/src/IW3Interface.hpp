#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Hooks.hpp"
#include "Events.hpp"
#include "DemoParser.hpp"
#include "Hooks/Camera.hpp"

namespace IWXMVM::IW3
{
	class IW3Interface : public GameInterface
	{
	public:

		IW3Interface() : GameInterface(Game::IW3) {}

		void InstallHooks() final
		{
			Hooks::Install();
		}

		void SetupEventListeners() final
		{
			Events::RegisterListener(EventType::OnDemoLoad, DemoParser::Run);
			Events::RegisterListener(EventType::OnCameraChanged, Hooks::Camera::OnCameraChanged);
		}

		uintptr_t GetWndProc() final
		{
			return (uintptr_t)0x57BB20;
		}

		void SetMouseMode(MouseMode mode) final
		{
			Structures::GetMouseVars()->mouseInitialized = (mode == MouseMode::Capture) ? false : true;
		}

		GameState GetGameState() final
		{
			if (!Structures::FindDvar("cl_ingame")->current.enabled)
				return GameState::MainMenu;

			if (Structures::GetClientConnection()->demoplaying)
				return GameState::InDemo;

			return GameState::InGame;
		}

		// TODO: cache this
		DemoInfo GetDemoInfo() final
		{
			DemoInfo demoInfo;
			demoInfo.name = Structures::GetClientStatic()->servername;

			std::string str = static_cast<std::string>(demoInfo.name);
			str += (str.ends_with(".dm_1")) ? "" : ".dm_1";
			demoInfo.path = Structures::GetFilePath(std::move(str));

			demoInfo.currentTick = Structures::GetClientActive()->serverTime - DemoParser::demoStartTick;
			demoInfo.endTick = DemoParser::demoEndTick - DemoParser::demoStartTick;

			return demoInfo;
		}

		std::string_view GetDemoExtension() final 
		{
			return { ".dm_1" };
		}

		std::filesystem::path GetDemoDirectory() final
		{
			return std::filesystem::path(GetDvar("fs_basepath")->value->string) / "players" / "demos";
		}

		void PlayHardlinkDemo()
		{
			Structures::Cbuf_AddText(
				std::format(R"(demo {0})", DEMO_HARDLINK_FILE_NAME)
			);
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

		std::optional<Dvar> GetDvar(const std::string name) final
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
