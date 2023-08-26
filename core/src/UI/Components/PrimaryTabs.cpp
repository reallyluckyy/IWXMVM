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

	// Only show icon if button is too small
	std::string ClipText(std::string text, ImVec2 buttonSize)
	{
		if (buttonSize.x < buttonSize.y * 2)
		{
			return text.find("  ") != std::string::npos ? text.substr(0, text.find(" ")) : text;
		}
		return text;
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
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
		if (ImGui::Begin("PrimaryTabs", nullptr, flags))
		{
			// Change these
			auto xMargin = GetSize().y / 9;
			auto yMargin = xMargin;

			auto totalXMargin = xMargin * (Tab::Count + 1);
			auto totalYMargin = yMargin * 2;
			auto buttonSize = ImVec2((GetSize().x - totalXMargin) / (float)Tab::Count, GetSize().y - totalYMargin);

			ImGui::SetCursorPosY(yMargin);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ClipText(ICON_FA_FILE "  DEMOS", buttonSize).c_str(), buttonSize))
				UIManager::selectedTab = Tab::Demos;

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ClipText(ICON_FA_VIDEO "  CAMERA", buttonSize).c_str(), buttonSize))
				UIManager::selectedTab = Tab::Camera;
			
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ClipText(ICON_FA_SLIDERS "  VISUALS", buttonSize).c_str(), buttonSize))
				UIManager::selectedTab = Tab::Visuals;

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			if (ImGui::Button(ClipText(ICON_FA_CIRCLE "  RECORD", buttonSize).c_str(), buttonSize))
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