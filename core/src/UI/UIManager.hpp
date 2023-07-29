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

	inline std::atomic<bool> isInitialized = false;
	inline std::atomic<bool> needsRestart = false;

	void Initialize(IDirect3DDevice9* device);
	bool RestartImGui();
}