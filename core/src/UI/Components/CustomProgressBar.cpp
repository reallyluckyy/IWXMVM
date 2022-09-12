#include "StdInclude.hpp"
#include "CustomProgressBar.hpp"

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
        const ImRect frame_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(frame_bb.Min, ImVec2(frame_bb.Max.x + label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, frame_bb.Max.y));

        ItemSize(total_bb, style.FramePadding.y);
        ItemAdd(total_bb, id, &frame_bb, 0);

        // Default format string when passing NULL
        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;

        const bool hovered = ItemHoverable(frame_bb, id);

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
}