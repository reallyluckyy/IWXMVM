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

namespace IWXMVM::UI
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

	class UIManager
	{

	public:
		static UIManager& Get()
		{
			static UIManager instance;
			return instance;
		}

		UIManager(UIManager const&) = delete;
		void operator=(UIManager const&) = delete;

		void Initialize(IDirect3DDevice9* device);
		void ShutdownImGui();
		bool RestartImGui();
		void RunImGuiFrame();

		bool IsInitialized() const
		{
			return isInitialized;
		}

		std::atomic<bool> const& NeedsRestart() const
		{
			return needsRestart;
		}

		WNDPROC GetOriginalGameWndProc() const
		{
			return originalGameWndProc;
		}

		std::unique_ptr<UIComponent> const& GetUIComponent(Component::Component component) const
		{
			return uiComponents[component];
		}

		std::array<std::unique_ptr<UIComponent>, Component::Count> const& GetUIComponents() const
		{
			return uiComponents;
		}

		void SelectTab(Tab tab)
		{
			selectedTab = tab;
		}

		Tab GetSelectedTab() const
		{
			return selectedTab;
		}

		void ToggleOverlay()
		{
			hideOverlay = !hideOverlay;
		}

		void ToggleImGuiDemo()
		{
			showImGuiDemo = !showImGuiDemo;
		}

		void ToggleDebugPanel()
		{
			showDebugPanel = !showDebugPanel;
		}

	private:
		UIManager() {}

		std::array<std::unique_ptr<UIComponent>, Component::Count> uiComponents =
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

		Tab selectedTab = Tab::Demos;
		bool isInitialized = false;

		std::atomic<bool> hideOverlay = false;
		std::atomic<bool> showImGuiDemo = false;
		std::atomic<bool> showDebugPanel = false;

		std::atomic<bool> needsRestart = false;

		WNDPROC originalGameWndProc = nullptr;
	};
}