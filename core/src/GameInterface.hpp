#pragma once
#include "StdInclude.hpp"
#include "D3D9Helper.hpp"
#include "Components/Camera.hpp"

namespace IWXMVM
{
	const auto DEMO_HARDLINK_FILE_NAME = "IWXMVM_DEMO_LINK";

	struct Dvar
	{
		std::string name;
		union Value
		{
			float floating_point;
			uint32_t uint32;
			int32_t int32;
			float vector[4];
			const char* string;
			uint8_t color[4];
		}* value;
	};

	class GameInterface
	{
	public:

		enum class Game
		{
			None, IW3
		};

		enum class GameState
		{
			MainMenu,
			InGame,
			InDemo
		};

		enum class MouseMode
		{
			Capture, Passthrough
		};

		GameInterface(const Game game) : game(game) {}
		virtual ~GameInterface() = default;

		Game GetGame() const { return game; }

		std::string GetGameName() const
		{
			switch (game)
			{
			case Game::IW3:
				return "IW3";
			case Game::None:
			default:
				return "Unknown Game";
			}
		}

		HWND GetWindowHandle() const { return D3D9Helper::FindWindowHandle(); }
		IDirect3DDevice9* GetD3D9Device() const { return D3D9Helper::GetDevicePtr(); }

		void HookD3D() const { D3D9Helper::Initialize(); }
		virtual void InstallHooks() = 0;
		virtual void SetupEventListeners() = 0;

		virtual uintptr_t GetWndProc() = 0;
		virtual void SetMouseMode(MouseMode mode) = 0;
		virtual GameState GetGameState() = 0;

		std::string GetGameStateString()
		{
			switch (GetGameState())
			{
			case GameState::MainMenu:
				return "Main Menu";
			case GameState::InDemo:
				return "Playing Demo";
			case GameState::InGame:
				return "In Game";
			default:
				return "Unknown Game";
			}
		}

		virtual std::filesystem::path GetDemoDirectory() = 0;
		std::filesystem::path GetDemoHardlinkPath() { return (GetDemoDirectory() / DEMO_HARDLINK_FILE_NAME).replace_extension(GetDemoExtension()); }

		struct DemoInfo
		{
			std::string name;
			std::string path;

			uint32_t currentTick;
			uint32_t endTick;
		};

		virtual DemoInfo GetDemoInfo() = 0;
		virtual std::string_view GetDemoExtension() = 0;

		virtual void PlayHardlinkDemo() = 0;

		void PlayDemo(std::filesystem::path demoPath)
		{
			try
			{
				LOG_INFO("Playing demo {0}", demoPath.string());

				if (!std::filesystem::exists(demoPath) || !std::filesystem::is_regular_file(demoPath))
					return;

				const auto demoDirectory = GetDemoDirectory();
				if (!std::filesystem::exists(demoDirectory))
					std::filesystem::create_directories(demoDirectory);

				const auto demoLinkFile = GetDemoHardlinkPath();
				if (std::filesystem::exists(demoLinkFile) && std::filesystem::is_regular_file(demoLinkFile))
					std::filesystem::remove(demoLinkFile);

				std::filesystem::create_hard_link(demoPath, demoLinkFile);

				PlayHardlinkDemo();
			}
			catch (std::filesystem::filesystem_error& e)
			{
				LOG_ERROR("Failed to create hard link for demo file {0}: {1}", demoPath.string(), e.what());
			}
		}

		virtual void ToggleDemoPlaybackState() = 0;
		virtual bool IsDemoPlaybackPaused() = 0;

		// perhaps dvars shouldnt be exposed to core at all?
		virtual std::optional<Dvar> GetDvar(const std::string name) = 0;

		virtual void SetPlayerModelVisible(bool visible) = 0;
		virtual void SetHUDVisible(bool visible) = 0;

	private:
		Game game;
	};
}
