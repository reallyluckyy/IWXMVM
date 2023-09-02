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
				UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().x,
				UIManager::Get().GetUIComponent(UI::Component::PrimaryTabs)->GetPosition().y + UIManager::Get().GetUIComponent(UI::Component::PrimaryTabs)->GetSize().y
			)
		);
		ImGui::SetNextWindowSize(
			ImVec2(
				ImGui::GetIO().DisplaySize.x - GetPosition().x,
				UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().y - UIManager::Get().GetUIComponent(UI::Component::PrimaryTabs)->GetSize().y
			)
		);

		switch (UIManager::Get().GetSelectedTab())
		{
			case Tab::Demos:
				UIManager::Get().GetUIComponent(UI::Component::DemoLoader)->Render();
				break;
			case Tab::Visuals:
				break;
			case Tab::Camera:
				break;
			case Tab::Record:
				UIManager::Get().GetUIComponent(UI::Component::CaptureMenu)->Render();
				break;

		}
	}

	void RenderTabButton(std::string text, ImVec2 size, Tab tab) 
	{
		// Only show icon if button is too small
		if (size.x < size.y * 2)
		{
			text = text.find("  ") != std::string::npos ? text.substr(0, text.find(" ")) : text;
		}

		if (UIManager::Get().GetSelectedTab() == tab)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(text.c_str(), size))
			{
				UIManager::Get().SelectTab(tab);
			}

			ImGui::PopStyleColor();
		}
		else 
		{
			if (ImGui::Button(text.c_str(), size))
			{
				UIManager::Get().SelectTab(tab);
			}
		}
	}

	void PrimaryTabs::Render()
	{
		SetPosition(
			UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().x,
			UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y
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
			RenderTabButton(ICON_FA_FILE "  DEMOS", buttonSize, Tab::Demos);

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			RenderTabButton(ICON_FA_VIDEO "  CAMERA", buttonSize, Tab::Camera);
			
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			RenderTabButton(ICON_FA_SLIDERS "  VISUALS", buttonSize, Tab::Visuals);

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xMargin);
			RenderTabButton(ICON_FA_CIRCLE "  RECORD", buttonSize, Tab::Record);
		}

		ImGui::End();

		ImGui::PopStyleVar(3);

		RenderSelectedTab();
	}

	void PrimaryTabs::Release()
	{

	}
}