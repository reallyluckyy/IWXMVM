#pragma once
#include "StdInclude.hpp"

#include "UIComponent.hpp"

#include "Components/Background.hpp"
#include "Components/ControlBar.hpp"
#include "Components/DebugPanel.hpp"
#include "Components/MenuBar.hpp"
#include "Components/GameView.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace IWXMVM::UI::UIManager
{
	inline std::vector<std::unique_ptr<UIComponent>> uiComponents = []() {
		std::vector<std::unique_ptr<UIComponent>> vec;

		// background should probably always come first
		vec.emplace_back(std::make_unique<Background>());
		vec.emplace_back(std::make_unique<GameView>());
		vec.emplace_back(std::make_unique<DebugPanel>());
		vec.emplace_back(std::make_unique<MenuBar>());
		vec.emplace_back(std::make_unique<ControlBar>());

		return vec;
	}();

	inline std::atomic<bool> ejectRequested = false;
	inline bool isInitialized = false;
	inline std::atomic<bool> needsRestart = false;

	inline WNDPROC GameWndProc = nullptr;

	void Initialize(IDirect3DDevice9* device);
	void ShutdownImGui();
	bool RestartImGui();
}