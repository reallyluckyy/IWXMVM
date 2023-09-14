#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"

namespace IWXMVM::UI
{
	void VisualsMenu::Initialize()
	{
		visuals = {
			reinterpret_cast<bool*>(&(Mod::GetGameInterface()->GetDvar("r_dof_tweak").value().value->uint32)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_farBlur").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_farEnd").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_farStart").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_nearBlur").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_nearStart").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_nearEnd").value().value->floating_point)),
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_bias").value().value->floating_point))
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
		ImGui::SliderFloat("Far Start", visuals.dofFarStart, 0, 5000);
		ImGui::SliderFloat("Far End", visuals.dofFarEnd, 0, 5000);

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		ImGui::SliderFloat("Near Blur", visuals.dofNearBlur, 0, 10);
		ImGui::SliderFloat("Near Start", visuals.dofNearStart, 0, 1000);
		ImGui::SliderFloat("Near End", visuals.dofNearEnd, 0, 1000);

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		ImGui::SliderFloat("Bias", visuals.dofBias, 0, 10);

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