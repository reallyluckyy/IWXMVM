#include "StdInclude.hpp"
#include "CustomImGuiControls.hpp"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui_internal.h"

namespace ImGui
{
	bool DemoProgressBar(uint32_t* currentTick, const uint32_t endTick, const ImVec2& size_arg, const char* overlay)
	{
		const char* label = "##demo_progress_bar";
		const char* format = "%d";
		ImGuiDataType data_type = ImGuiDataType_U32;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = size_arg.x;

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		ItemSize(total_bb, style.FramePadding.y);
		ItemAdd(total_bb, id, &frame_bb, 0);

		// Default format string when passing NULL
		if (format == NULL)
			format = DataTypeGetInfo(data_type)->PrintFmt;

		const bool hovered = ItemHoverable(frame_bb, id, ImGuiItemFlags_None);

		// Draw frame
		const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavHighlight(frame_bb, id);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, false, g.Style.FrameRounding);

		// Slider behavior
		ImRect grab_bb;
		int minValue = 0;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, currentTick, &minValue, &endTick, format, 0, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);

		grab_bb.Min.y = frame_bb.Min.y;
		grab_bb.Max.y = frame_bb.Max.y;
		grab_bb.Max.x = grab_bb.Min.x + (grab_bb.Max.x - grab_bb.Min.x) / 2;

		window->DrawList->AddRectFilled(frame_bb.Min, grab_bb.Max, GetColorU32(ImGuiCol_PlotHistogram));

		constexpr auto MARKER_DISTANCE = 5000;
		for (uint32_t i = MARKER_DISTANCE; i < *currentTick; i += MARKER_DISTANCE)
		{
			auto barLength = frame_bb.Max.x - frame_bb.Min.x;
			auto percentage = (float)i / (float)endTick;
			auto x = frame_bb.Min.x + percentage * barLength;
			window->DrawList->AddRectFilled(ImVec2(x, frame_bb.Min.y), ImVec2(x + 2, frame_bb.Max.y), GetColorU32(ImGuiCol_Button));
		}

		// Render grab
		if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		ImVec2 overlay_size = CalcTextSize(overlay, NULL);
		RenderTextClipped(ImVec2(frame_bb.Max.x + style.ItemSpacing.x, frame_bb.Min.y), ImVec2(frame_bb.Max.x + 200, frame_bb.Max.y), overlay, NULL, NULL, ImVec2(0.00f, 0.5f));

		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return value_changed;
	}

	constexpr std::array timescaleSteps{ 0.001, 0.002, 0.005, 0.01, 0.02, 0.025, 0.05, 0.075, 0.1, 0.125, 0.2, 0.25, 0.333, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 500.0, 1000.0 };

	bool TimescaleSliderInternal(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
			return false;

		// Default format string when passing NULL
		if (format == NULL)
			format = DataTypeGetInfo(data_type)->PrintFmt;

		// Tabbing or CTRL-clicking on Slider turns it into an input box
		const bool hovered = ItemHoverable(frame_bb, id, ImGuiItemFlags_None);
		bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
		if (!temp_input_is_active) 
		{
			const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
			const bool clicked = (hovered && g.IO.MouseClicked[0]);
			if (input_requested_by_tabbing || clicked || g.NavActivateId == id) 
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
				if (temp_input_allowed && (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl)))
					temp_input_is_active = true;
			}
		}

		if (temp_input_is_active) 
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
			return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
		}

		// Draw frame
		const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavHighlight(frame_bb, id);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

		// Slider behavior
		ImRect grab_bb;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed) 
		{
			MarkItemEdited(id);

			static constexpr double rounding = 0.00000001;
			float* ptr = static_cast<float*>(p_data);

			for (std::size_t i = 1; i < timescaleSteps.size(); ++i) 
			{
				if (*ptr > timescaleSteps[i - 1] + rounding && *ptr < timescaleSteps[i] - rounding) 
				{
					*ptr = (float)timescaleSteps[i - 1];
					break;
				}
			}
		}

		// Render grab
		if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
		if (g.LogEnabled)
			LogSetNextTextDecoration("{", "}");
		RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return value_changed;
	}

	bool TimescaleSlider(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return TimescaleSliderInternal(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}
}