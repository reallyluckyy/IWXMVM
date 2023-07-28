#pragma once

#include "StdInclude.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace IWXMVM::UI::UIManager
{
	enum class InitType
	{
		Initialize,
		Reinitialize
	};

	inline std::atomic<bool> isInitialized = false;
	inline std::atomic<bool> needsRestart = false;

	void Initialize(InitType type, IDirect3DDevice9* d3dDevice);
}