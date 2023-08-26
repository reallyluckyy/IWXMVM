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

	void PrimaryTabs::RenderSelectedTab()
	{
		ImGui::SetNextWindowPos(
			ImVec2(
				UIManager::uiComponents[UIManager::Component::GameView]->GetSize().x,
				UIManager::uiComponents[UIManager::Component::PrimaryTabs]->GetPosition().y + UIManager::uiComponents[UIManager::Component::PrimaryTabs]->GetSize().y
			)
		);
		ImGui::SetNextWindowSize(
			ImVec2(
				ImGui::GetIO().DisplaySize.x - GetPosition().x,
				UIManager::uiComponents[UIManager::Component::GameView]->GetSize().y - UIManager::uiComponents[UIManager::Component::PrimaryTabs]->GetSize().y
			)
		);

		switch (UIManager::selectedTab)
		{
			case Tab::Demos:
				UIManager::uiComponents[UIManager::Component::DemoLoader]->Render();
				break;
			case Tab::Visuals:
				break;
			case Tab::Camera:
				break;
			case Tab::Record:
				UIManager::uiComponents[UIManager::Component::CaptureMenu]->Render();
				break;

		}
	}

	void PrimaryTabs::Render()
	{
		SetPosition(
			UIManager::uiComponents[UIManager::Component::GameView]->GetSize().x,
			UIManager::uiComponents[UIManager::Component::MenuBar]->GetSize().y
		);
		SetSize(
			ImGui::GetIO().DisplaySize.x - GetPosition().x,
			ImGui::GetFrameHeight() * 2.4f
		);

		ImGui::SetNextWindowPos(GetPosition());
		ImGui::SetNextWindowSize(GetSize());

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
		if (ImGui::Begin("PrimaryTabs", nullptr, flags))
		{
			// Change these
			auto xMargin = GetSize().y / 9;
			auto yMargin = xMargin;

			auto totalXMargin = xMargin * (Tab::Count + 1);
			auto totalYMargin = yMargin * 2;
			auto size = ImVec2((GetSize().x - totalXMargin) / (float)Tab::Count, GetSize().y - totalYMargin);

			ImGui::SetCursorPosY(yMargin);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ICON_FA_FILE "  DEMOS", size))
				UIManager::selectedTab = Tab::Demos;

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ICON_FA_VIDEO "  CAMERA", size))
				UIManager::selectedTab = Tab::Camera;
			
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ICON_FA_SLIDERS "  VISUALS", size))
				UIManager::selectedTab = Tab::Visuals;

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ICON_FA_CIRCLE "  RECORD", size))
				UIManager::selectedTab = Tab::Record;
		}

		ImGui::End();

		ImGui::PopStyleVar(3);

		RenderSelectedTab();
	}

	void PrimaryTabs::Release()
	{

	}
}