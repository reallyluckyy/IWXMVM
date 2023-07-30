#pragma once
#include "StdInclude.hpp"
#include "GameInterface.hpp"

#include "Structures.hpp"
#include "Hooks.hpp"
#include "Events.hpp"
#include "DemoParser.hpp"

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
			demoInfo.name = Structures::GetClientConnection()->demoName;

			std::string str = static_cast<std::string>(demoInfo.name);
			str += (str.ends_with(".dm_1")) ? "" : ".dm_1";
			demoInfo.path = Structures::GetFilePath(std::move(str));

			demoInfo.currentTick = Structures::GetClientActive()->serverTime - DemoParser::demoStartTick;
			demoInfo.endTick = DemoParser::demoEndTick - DemoParser::demoStartTick;

			return demoInfo;
		}

		std::vector<std::string> GetDemoExtensions() final 
		{
			return { ".dm_1" };
		}

		void PlayDemo(std::filesystem::path demoPath) final
		{
			try 
			{
				LOG_INFO("Playing demo {0}", demoPath.string());

				if (!std::filesystem::exists(demoPath) || !std::filesystem::is_regular_file(demoPath))
					return;

				auto tempDemoDir = std::filesystem::path(GetDvar("fs_basepath")->value->string) / "players" / "demos" / DEMO_TEMP_DIR_NAME;
				if (!std::filesystem::exists(tempDemoDir))
				{
					std::filesystem::create_directories(tempDemoDir);
				}

				auto targetDemoFilePath = tempDemoDir / demoPath.filename();
				if (demoPath != targetDemoFilePath)
				{
					if (std::filesystem::exists(targetDemoFilePath) && std::filesystem::is_regular_file(targetDemoFilePath)) {
						std::filesystem::remove(targetDemoFilePath);
					}
					LOG_DEBUG("Copying demo file from {0} to {1}", demoPath.string(), targetDemoFilePath.string());
					std::filesystem::copy_file(demoPath, targetDemoFilePath);
				}

				Structures::Cbuf_AddText(
					std::format("demo \"{0}/{1}\"", 
						DEMO_TEMP_DIR_NAME,
						demoPath.filename().string()
					)
				);
			}
			catch (std::filesystem::filesystem_error& e) 
			{
				LOG_ERROR("Failed to copy demo file {0}: {1}", demoPath.string(), e.what());
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