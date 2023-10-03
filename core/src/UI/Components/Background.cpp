#include "StdInclude.hpp"
#include "Background.hpp"

#include "Mod.hpp"
#include "imgui_internal.h"

namespace IWXMVM::UI
{
void Background::Initialize()
{
    SetPosition(0, 0);
    SetSize(ImGui::GetIO().DisplaySize);
}

void Background::Render()
{
    ImGui::SetNextWindowPos(GetPosition());
    ImGui::SetNextWindowSize(GetSize());

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin("Background", NULL,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->DrawList->PushClipRectFullScreen();
    window->DrawList->AddRectFilled(ImVec2(0, 0), GetSize(), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0,
                                    ImDrawCornerFlags_None);

    ImGui::End();
    ImGui::PopStyleColor();
}

void Background::Release()
{
}
}  // namespace IWXMVM::UI