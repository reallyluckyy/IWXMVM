#include "StdInclude.hpp"
#include "CampathMenu.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Components/CameraManager.hpp"
#include "Input.hpp"

namespace IWXMVM::UI
{
	void CampathMenu::Initialize()
	{
	}

	void CampathMenu::Render()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Campath", NULL, flags))
		{
			ImGui::Text("Campath Menu goes here");
			ImGui::End();
		}

		if (Mod::GetGameInterface()->GetGameState() == Types::GameState::InDemo 
			&& Components::CameraManager::Get().GetActiveCamera()->GetMode() != Components::Camera::Mode::Dolly)
		{
			RenderHotkeyOverlay();
		}

		auto& campathManager = Components::CampathManager::Get();

		ImGui::AlignTextToFramePadding();
		if (ImGui::BeginCombo("##campathMenuInterpolationCombo", campathManager.GetInterpolationModeLabel(campathManager.GetInterpolationMode()).data()))
		{
			for (auto interpolationMode : campathManager.GetInterpolationModes())
			{
				bool isSelected = campathManager.GetInterpolationMode() == interpolationMode;
				if (ImGui::Selectable(campathManager.GetInterpolationModeLabel(interpolationMode).data(), campathManager.GetInterpolationMode() == interpolationMode))
				{
					campathManager.SetInterpolationMode(interpolationMode);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void DrawKeyLine(Key key, std::string_view label, ImVec2 position)
	{
		auto text = std::format("{} - {}", ImGui::GetKeyName(key), label);
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), position, ImGui::GetColorU32(ImGuiCol_Text), text.data());
	}

	void CampathMenu::RenderHotkeyOverlay()
	{
		// Drawing things "directly" since there doesnt seem to be a way to create a window thats always on top...

		auto overlaySize = ImVec2(ImGui::GetFontSize() * 14, ImGui::GetFontSize() * 7);
		auto overlayMargin = ImVec2(ImGui::GetFontSize() * 1, ImGui::GetFontSize() * 1);
		auto overlayPadding = ImVec2(ImGui::GetFontSize() * 0.75f, ImGui::GetFontSize() * 0.75f);
		auto lineHeight = 1.15f * ImGui::GetFontSize();

		auto& gameView = UIManager::Get().GetUIComponent(Component::GameView);
		auto basePos = gameView->GetPosition() + gameView->GetSize() - overlaySize - overlayMargin;
		ImGui::GetForegroundDrawList()->AddRectFilled(basePos, basePos + overlaySize, ImGui::GetColorU32(ImGuiCol_WindowBg));
		ImGui::GetForegroundDrawList()->AddRect(basePos, basePos + overlaySize, ImGui::GetColorU32(ImGuiCol_Border));
		auto contentBasePos = basePos + overlayPadding;

		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), contentBasePos, ImGui::GetColorU32(ImGuiCol_Text), "Campath Hotkeys");

		DrawKeyLine(InputConfiguration::Get().GetKeyBind(InputConfiguration::BIND_DOLLY_ADD_NODE), "Add Node", contentBasePos + ImVec2(0, lineHeight * 1.5));
		DrawKeyLine(InputConfiguration::Get().GetKeyBind(InputConfiguration::BIND_DOLLY_PLAY_PATH), "Switch To Dollycam", contentBasePos + ImVec2(0, lineHeight * 1.5 + lineHeight));
		DrawKeyLine(InputConfiguration::Get().GetKeyBind(InputConfiguration::BIND_DOLLY_CLEAR_NODES), "Delete All Nodes", contentBasePos + ImVec2(0, lineHeight * 1.5 + lineHeight * 2));
	}

	void CampathMenu::Release()
	{
	}
}