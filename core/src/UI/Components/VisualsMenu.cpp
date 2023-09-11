#include "StdInclude.hpp"
#include "VisualsMenu.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
	void VisualsMenu::Initialize()
	{

	}

	void VisualsMenu::Render()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Visuals", NULL, flags))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Visuals:");
			

			ImGui::End();
		}
	}

	void VisualsMenu::Release()
	{
	}
}