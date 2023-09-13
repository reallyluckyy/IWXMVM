#include "StdInclude.hpp"
#include "VisualsMenu.hpp"
#include "Mod.hpp"

#include "UI/UIManager.hpp"
#include "glm/vec3.hpp"

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
			reinterpret_cast<float*>(&(Mod::GetGameInterface()->GetDvar("r_dof_bias").value().value->floating_point)),
			initColors,
			initPosition[Pos::x],
			initPosition[Pos::y],
			initPosition[Pos::z]
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

		if (ImGui::ColorEdit3("Color", visuals.sunColorUI))
			UpdateSunColor();

		if (ImGui::SliderFloat("Brightness", &visuals.sunBrightness, 0, 4))
			UpdateSunColor();

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		if (ImGui::SliderFloat("Pitch", &visuals.sunPitch, 0, 360))
			UpdateSunAngle();

		if (ImGui::SliderFloat("Yaw", &visuals.sunYaw, 0, 360))
			UpdateSunAngle();

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spacing

		if (ImGui::SliderFloat("X", &visuals.sunPositionXUI, -1, 1))
			UpdateSunPosition(Pos::x);

		if (ImGui::SliderFloat("Y", &visuals.sunPositionYUI, -1, 1))
			UpdateSunPosition(Pos::y);

		if (ImGui::SliderFloat("Z", &visuals.sunPositionZUI, -1, 1))
			UpdateSunPosition(Pos::z);


		ImGui::Separator();
	}

	void VisualsMenu::UpdateSunColor()
	{
		if (sunColor == (float*)0x4 && Mod::GetGameInterface()->GetSun()->color != (float*)0x4)
			sunColor = Mod::GetGameInterface()->GetSun()->color;
		else if (sunColor != Mod::GetGameInterface()->GetSun()->color)
			sunColor = Mod::GetGameInterface()->GetSun()->color;
		
		if (sunColor != (float*)0x4)
		{
			for (int i = 0; i < 3; ++i)
				sunColor[i] = visuals.sunColorUI[i] * visuals.sunBrightness;
		}
	}

	void VisualsMenu::UpdateSunPosition(int axis)
	{
		if (sunPosition == (float*)0x10 && Mod::GetGameInterface()->GetSun()->position != (float*)0x10)
			sunPosition = Mod::GetGameInterface()->GetSun()->position;
		else if (sunPosition != Mod::GetGameInterface()->GetSun()->position)
			sunPosition = Mod::GetGameInterface()->GetSun()->position;

		if (sunPosition != (float*)0x10)
		{
			switch (axis)
			{
				case Pos::x:
					sunPosition[axis] = visuals.sunPositionXUI;
					break;
				case Pos::y:
					sunPosition[axis] = visuals.sunPositionYUI;
					break;
				case Pos::z:
					sunPosition[axis] = visuals.sunPositionZUI;
					break;
			}
		}
	}

	void VisualsMenu::UpdateSunAngle()
	{
		constexpr double pi = 3.14159265358979323846;
		float origin = 0;
		float radius = 1;
		glm::vec3 rotation = { visuals.sunPitch, visuals.sunYaw ,0 };
		
		visuals.sunPositionXUI = (float)(origin + radius * cos(2*pi*(rotation.y/360)) * cos(2*pi*(rotation.x/360)));
		visuals.sunPositionYUI = (float)(origin + radius * sin(2 * pi * (rotation.y / 360)) * cos(2 * pi * (rotation.z / 360)));
		visuals.sunPositionZUI = -(float)(origin + radius * sin(2 * pi * (rotation.x / 360)));
		UpdateSunPosition(Pos::x);
		UpdateSunPosition(Pos::y);
		UpdateSunPosition(Pos::z);
	}

	void VisualsMenu::Release()
	{
	}
}