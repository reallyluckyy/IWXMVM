#include "StdInclude.hpp"
#include "Background.hpp"

#include "Mod.hpp"
#include "imgui_internal.h"

namespace IWXMVM::UI
{
	void Background::Initialize()
	{}

	void Background::Render()
	{

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("Background", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::SetWindowPos(ImVec2(0, 0), 0);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.x), 0);

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		window->DrawList->AddRectFilled(ImVec2(0, 0), ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.x), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0, ImDrawCornerFlags_None);

		ImGui::End();
		ImGui::PopStyleColor();
	}

	void Background::Release()
	{}
}
