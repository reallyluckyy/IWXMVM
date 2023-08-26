#pragma once
#include "StdInclude.hpp"

#include "UIComponent.hpp"

#include "Components/Background.hpp"
#include "Components/CaptureMenu.hpp"
#include "Components/ControlBar.hpp"
#include "Components/DebugPanel.hpp"
#include "Components/DemoLoader.hpp"
#include "Components/GameView.hpp"
#include "Components/MenuBar.hpp"
#include "Components/PrimaryTabs.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace IWXMVM::UI::UIManager
{
	namespace Component 
	{
		enum Component 
		{
			Background = 0,
			MenuBar,
			GameView,
			PrimaryTabs,
			DemoLoader,
			CaptureMenu,
			ControlBar,
			DebugPanel,

			Count,
		};
	}

	inline std::array<std::unique_ptr<UIComponent>, Component::Count> uiComponents = 
	{
		std::make_unique<Background>(),
		std::make_unique<MenuBar>(),
		std::make_unique<GameView>(),
		std::make_unique<PrimaryTabs>(),
		std::make_unique<DemoLoader>(),
		std::make_unique<CaptureMenu>(),
		std::make_unique<ControlBar>(),
		std::make_unique<DebugPanel>(),
	};

	inline Tab selectedTab = Tab::Demos;

	inline std::atomic<bool> hideOverlay = false;
	inline std::atomic<bool> showImGuiDemo = false;
	inline std::atomic<bool> showDebugPanel = false;

	inline std::atomic<bool> ejectRequested = false;
	inline bool isInitialized = false;
	inline std::atomic<bool> needsRestart = false;

	inline WNDPROC GameWndProc = nullptr;

	void Initialize(IDirect3DDevice9* device);
	void ShutdownImGui();
	bool RestartImGui();
}