#include "StdInclude.hpp"
#include "DebugPanel.hpp"

#include "Utilities/HookManager.hpp"
#include "UI/UIManager.hpp"
#include "Mod.hpp"

namespace IWXMVM::UI
{
	void DebugPanel::Initialize()
	{

	}

	void DebugPanel::Render()
	{
		if (ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Game State: %s", Mod::GetGameInterface()->GetGameStateString().c_str());
			ImGui::Text("Demo Name: %s", Mod::GetGameInterface()->GetDemoInfo().name.c_str());
			ImGui::Text("Demo Tick: %d", Mod::GetGameInterface()->GetDemoInfo().currentTick);
			ImGui::Text("Demo End Tick: %d", Mod::GetGameInterface()->GetDemoInfo().endTick);
			auto& camera = Mod::GetCameraManager()->GetActiveCamera();
			ImGui::Text("Camera: %f %f %f", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
			if (ImGui::Button("Eject"))
				UIManager::ejectRequested.store(true);
			ImGui::End();
		}
	}

	void DebugPanel::Release()
	{}
}