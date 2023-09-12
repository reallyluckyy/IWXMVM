#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
	void VisualsMenu::Initialize()
	{
		//Mod::GetGameInterface()->GetDvar("r_dof_tweak").value().value->uint32 = 1;
		visuals = {
			reinterpret_cast<bool*>(&(Mod::GetGameInterface()->GetDvar("r_dof_tweak").value().value->uint32)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_farBlur").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_farEnd").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_farStart").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_nearBlur").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_nearStart").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_nearEnd").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_bias").value().value->floating_point)),
			Mod::GetGameInterface()->GetSun()->color,
			Mod::GetGameInterface()->GetSun()->position
		};
	}

	void VisualsMenu::Render()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Visuals", NULL, flags))
		{
			RenderDOF();
			RenderSun();
			
			ImGui::End();
		}
	}

	void VisualsMenu::RenderDOF()
	{
		ImGui::AlignTextToFramePadding();

		ImGui::Text("DOF:");

		ImGui::Checkbox("Enable DOF", visuals.dofActive);
		ImGui::SliderFloat("Far Blur", visuals.dofFarBlur, 0, 10);
		ImGui::SliderFloat("Far Start", visuals.dofFarStart, 0, 20000);
		ImGui::SliderFloat("Far End", visuals.dofFarEnd, 0, 10000);

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Seperator

		ImGui::SliderFloat("Near Blur", visuals.dofNearBlur, 0, 10);
		ImGui::SliderFloat("Near Start", visuals.dofNearStart, 0, 1000);
		ImGui::SliderFloat("Near End", visuals.dofNearEnd, 0, 1000);

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Seperator

		ImGui::SliderFloat("Bias", visuals.dofBias, 0, 10);

		ImGui::Separator();
	}

	void VisualsMenu::RenderSun()
	{
		ImGui::AlignTextToFramePadding();

		ImGui::Text("Sun:");

		if (visuals.sunColor == (float*)0x4)
		{
			if (Mod::GetGameInterface()->GetSun()->color != (float*)0x4)
			{
				visuals.sunColor = Mod::GetGameInterface()->GetSun()->color;
				ImGui::ColorEdit3("Color", visuals.sunColor);
			}
		}
		else if (visuals.sunColor != Mod::GetGameInterface()->GetSun()->color)
		{
			visuals.sunColor = Mod::GetGameInterface()->GetSun()->color;
			ImGui::ColorEdit3("Color", visuals.sunColor);
		}
		else
			ImGui::ColorEdit3("Color", visuals.sunColor);

		ImGui::Separator();
	}

	void VisualsMenu::Release()
	{
	}
}