#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

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
			RenderDOF();
			RenderSun();
			
			ImGui::End();
		}
	}

	void VisualsMenu::RenderDOF()
	{
		ImGui::AlignTextToFramePadding();

		ImGui::Text("DOF:");

		if (ImGui::Checkbox("Enable DOF", &visuals.dofActive))
		{
			dof.enabled = visuals.dofActive;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::enabled);
		}

		if (ImGui::SliderFloat("Far Blur", &visuals.dofFarBlur, 0, 10))
		{
			dof.farBlur = visuals.dofFarBlur;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::farBlur);
		}
		if (ImGui::SliderFloat("Far Start", &visuals.dofFarStart, 0, 5000))
		{
			dof.farStart = visuals.dofFarStart;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::farStart);
		}
		if (ImGui::SliderFloat("Far End", &visuals.dofFarEnd, 0, 5000))
		{
			dof.farEnd = visuals.dofFarEnd;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::farEnd);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		if (ImGui::SliderFloat("Near Blur", &visuals.dofNearBlur, 0, 10))
		{
			dof.nearBlur = visuals.dofNearBlur;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::nearBlur);
		}
		if (ImGui::SliderFloat("Near Start", &visuals.dofNearStart, 0, 5000))
		{
			dof.nearStart = visuals.dofNearStart;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::nearStart);
		}
		if (ImGui::SliderFloat("Near End", &visuals.dofNearEnd, 0, 5000))
		{
			dof.nearEnd = visuals.dofNearEnd;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::nearEnd);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		if (ImGui::SliderFloat("Bias", &visuals.dofBias, 0, 10))
		{
			dof.bias = visuals.dofBias;
			Mod::GetGameInterface()->SetDof(dof, IWXMVM::Types::DoFSetting::bias);
		}

		ImGui::Separator();
	}

	void VisualsMenu::RenderSun()
	{
		ImGui::AlignTextToFramePadding();

		ImGui::Text("Sun:");

		if (ImGui::ColorEdit3("Color", glm::value_ptr(visuals.sunColorUI)))
			UpdateSun();

		if (ImGui::SliderFloat("Brightness", &visuals.sunBrightness, 0, 4))
			UpdateSun();

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		// ImGui::SliderAngle sets sun angles in radians but displays them as degrees
		if (ImGui::SliderAngle("Pitch", &visuals.sunPitch, 0, 360))
			UpdateSunAngle();

		if (ImGui::SliderAngle("Yaw", &visuals.sunYaw, 0, 360))
			UpdateSunAngle();

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		if (ImGui::SliderFloat("X", glm::value_ptr(visuals.sunPositionUI), -1, 1))
			UpdateSun();

		if (ImGui::SliderFloat("Y", &(glm::value_ptr(visuals.sunPositionUI)[1]), -1, 1))
			UpdateSun();

		if (ImGui::SliderFloat("Z", &(glm::value_ptr(visuals.sunPositionUI)[2]), -1, 1))
			UpdateSun();


		ImGui::Separator();
	}

	void VisualsMenu::UpdateSun()
	{
		IWXMVM::Types::Sun sun = { glm::make_vec3(visuals.sunColorUI), glm::make_vec3(visuals.sunPositionUI), visuals.sunBrightness };
		Mod::GetGameInterface()->SetSun(sun);
	}

	void VisualsMenu::UpdateSunAngle()
	{
		constexpr double pi = 3.14159265358979323846;
		float origin = 0;
		float radius = 1;
		glm::vec3 rotation = { visuals.sunPitch, visuals.sunYaw ,0 };
		
		visuals.sunPositionUI.x = (float)(origin + radius * cos(rotation.y) * cos(rotation.x));
		visuals.sunPositionUI.y = (float)(origin + radius * sin(rotation.y) * cos(rotation.z));
		visuals.sunPositionUI.z = -(float)(origin + radius * sin(rotation.x));

		UpdateSun();
	}

	void VisualsMenu::Release()
	{
	}
}