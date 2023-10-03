#pragma once

#include "Types/CampathNode.hpp"
#include "Components/Camera.hpp"

namespace ImGuiEx
{
	void DrawLine3D(glm::vec3 from, glm::vec3 to, ImVec4 color, float thickness);
	void DrawPoint3D(glm::vec3 point, ImVec4 color, ImVec2 size = ImVec2(3, 3));

	void TimelineMarkers(const std::vector<IWXMVM::Types::CampathNode>& nodes, std::uint32_t endTick);
	void DemoProgressBarLines(std::uint32_t currentTick, std::uint32_t endTick);
	bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags);
}
