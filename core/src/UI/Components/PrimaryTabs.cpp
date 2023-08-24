#include "StdInclude.hpp"
#include "PrimaryTabs.hpp"

#include "Resources.hpp"
#include "UI/UIImage.hpp"
#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
	void PrimaryTabs::Initialize()
	{

	}

	INCBIN_EXTERN(CFG_ICON);
	INCBIN_EXTERN(DEMOS_ICON);
	INCBIN_EXTERN(CAPTURE_ICON);
	INCBIN_EXTERN(DEBUG_ICON);

	void PrimaryTabs::Render()
	{
		SetPosition(
			UIManager::uiComponents[UIManager::GAMEVIEW]->GetSize().x,
			UIManager::uiComponents[UIManager::MENUBAR]->GetSize().y
		);
		SetSize(
			ImGui::GetIO().DisplaySize.x - GetPosition().x,
			ImGui::GetFrameHeight() * 3.6f
		);

		ImGui::SetNextWindowPos(GetPosition());
		ImGui::SetNextWindowSize(GetSize());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
		if (ImGui::Begin("PrimaryTabs", nullptr, flags))
		{
			auto size = GetSize();

			auto cfgIcon = UIImage::FromResource(CFG_ICON_data, CFG_ICON_size);
			if (ImGui::ImageButton(cfgIcon.GetTextureID(), ImVec2(size.y, size.y), ImVec2(-0.3, -0.3), ImVec2(1.3, 1.3), 0, ImVec4(0.054901f, 0.054901f, 0.054901f, 1)))
				UIManager::selectedTab = Tab::CFG;

			auto demosIcon = UIImage::FromResource(DEMOS_ICON_data, DEMOS_ICON_size);
			ImGui::SetCursorPosX(size.x / 4 + size.x / 8 - size.y / 2);
			ImGui::SetCursorPosY(0);
			if (ImGui::ImageButton(demosIcon.GetTextureID(), ImVec2(size.y, size.y), ImVec2(-0.3, -0.3), ImVec2(1.3, 1.3), 0, ImVec4(0.054901f, 0.054901f, 0.054901f, 1)))
				UIManager::selectedTab = Tab::DEMOS;

			auto captureIcon = UIImage::FromResource(CAPTURE_ICON_data, CAPTURE_ICON_size);
			ImGui::SetCursorPosX(size.x / 4 * 2 + size.x / 8 - size.y / 2);
			ImGui::SetCursorPosY(0);
			if (ImGui::ImageButton(captureIcon.GetTextureID(), ImVec2(size.y, size.y), ImVec2(-0.3, -0.3), ImVec2(1.3, 1.3), 0, ImVec4(0.054901f, 0.054901f, 0.054901f, 1)))
				UIManager::selectedTab = Tab::CAPTURE;

			auto debugIcon = UIImage::FromResource(DEBUG_ICON_data, DEBUG_ICON_size);
			ImGui::SetCursorPosX(size.x - size.y);
			ImGui::SetCursorPosY(0);
			if (ImGui::ImageButton(debugIcon.GetTextureID(), ImVec2(size.y, size.y), ImVec2(-0.3, -0.3), ImVec2(1.3, 1.3), 0, ImVec4(0.054901f, 0.054901f, 0.054901f, 1)))
				UIManager::selectedTab = Tab::DEBUG;

			ImGui::End();
		}
	}

	void PrimaryTabs::Release()
	{

	}
}