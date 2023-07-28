#pragma once
#include "StdInclude.hpp"

namespace IWXMVM
{
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
		std::string GetGameWindowName() const
		{
			switch (game)
			{
			case Game::IW3:
				return "Call of Duty 4";
			case Game::None:
				return "";
			}
		}

		IDirect3DDevice9* GetD3D9Device() const { return device; }
		void SetD3D9Device(IDirect3DDevice9* d3dDevice) { device = d3dDevice; }
		HWND GetWindowHandle() const { return hwnd; }
		void** GetVTable() const { return vTable; }

		void HookD3D()
		{
			hwnd = FindWindow(0, GetGameWindowName().c_str());
			if (!hwnd)
				throw std::runtime_error("Failed to find HWND");

			IDirect3D9* d3dObj = Direct3DCreate9(D3D_SDK_VERSION);
			if (!d3dObj)
				throw std::runtime_error("Failed to create D3D object");

			IDirect3DDevice9* dummyDevice = nullptr;

			D3DPRESENT_PARAMETERS d3d_params = {};
			d3d_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3d_params.hDeviceWindow = hwnd;
			d3d_params.Windowed = true;

			// Try to create device - will fail if in fullscreen
			HRESULT result = d3dObj->CreateDevice(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				d3d_params.hDeviceWindow,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
				&d3d_params,
				&dummyDevice
			);

			// Try again in case it's fullscreen
			if (FAILED(result) || !dummyDevice) {
				d3d_params.Windowed = false;
				result = d3dObj->CreateDevice(
					D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
					d3d_params.hDeviceWindow,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
					&d3d_params,
					&dummyDevice
				);
			}

			// Fail again -> death
			if (FAILED(result) || !dummyDevice) {
				d3dObj->Release();
				throw std::runtime_error("Failed to create dummy D3D device");
			}

			// Deallocate vtable if reinitializing
			if (vTable)
				free(vTable);

			vTable = (void**)calloc(119, sizeof(void*));
			if (!vTable) {
				dummyDevice->Release();
				d3dObj->Release();
				throw std::runtime_error("calloc failed");
			}
			memcpy(vTable, *(void**)dummyDevice, 119 * sizeof(void*));

			LOG_DEBUG("Created dummy D3D device");

			dummyDevice->Release();
			d3dObj->Release();
		}

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


		struct DemoInfo
		{
			std::string name;
			std::string path;

			uint32_t currentTick;
			uint32_t endTick;
		};

		virtual DemoInfo GetDemoInfo() = 0;


		virtual void ToggleDemoPlaybackState() = 0;
		virtual bool IsDemoPlaybackPaused() = 0;

		virtual std::optional<Dvar> GetDvar(const std::string name) = 0;

	private:
		Game game;
		HWND hwnd = nullptr;
		IDirect3DDevice9* device = nullptr;
		void** vTable = nullptr;
	};
}