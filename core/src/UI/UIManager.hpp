#pragma once

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace IWXMVM::UI::UIManager
{
	enum class InitType
	{
		Initialize,
		Reinitialize
	};

	void Initialize(InitType type);
}