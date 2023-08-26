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

		IW3Interface() : GameInterface(Types::Game::IW3) {}

		void InstallHooks() final
		{
			Hooks::Install();
		}

		void SetupEventListeners() final
		{
			Events::RegisterListener(EventType::OnDemoLoad, DemoParser::Run);

			Events::RegisterListener(EventType::OnCameraChanged, Hooks::Camera::OnCameraChanged);
			
      Events::RegisterListener(EventType::OnDemoLoad, []() { Structures::FindDvar("sv_cheats")->current.enabled = true; });
		}

		uintptr_t GetWndProc() final
		{
			return (uintptr_t)0x57BB20;
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

		void PlayDemo(std::filesystem::path demoPath) final
		{
			const auto demoDirectory = std::filesystem::path(GetDvar("fs_basepath")->value->string) / "players" / "demos";

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
					std::format(R"(demo {0})", demoPath.filename().string())
				);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				LOG_ERROR("Failed to play demo file {0}: {1}", demoPath.string(), e.what());
			}
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
