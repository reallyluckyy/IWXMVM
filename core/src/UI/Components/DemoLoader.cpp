#include "StdInclude.hpp"
#include "DemoLoader.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{

	void DemoLoader::FindAllDemos()
	{
		discoveredDemoPaths.clear();

		for (std::filesystem::recursive_directory_iterator i(PathUtils::GetCurrentGameDirectory()), end; i != end; ++i)
		{
			auto demoExtensions = Mod::GetGameInterface()->GetDemoExtensions();

			if (!std::filesystem::is_directory(i->path()) && i->path().has_extension() && std::find(demoExtensions.begin(), demoExtensions.end(), i->path().extension().string()) != demoExtensions.end())
			{
				discoveredDemoPaths.push_back(i->path());
			}
		}
	}

	void DemoLoader::Initialize()
	{
	}

	void DemoLoader::Render()
	{
		if (!initiallyLoadedDemos) 
		{
			FindAllDemos();
			initiallyLoadedDemos = true;
		}

		ImGui::Begin("Demos", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		
		ImGui::Text("%d demos found!", discoveredDemoPaths.size()); 
		ImGui::SameLine();
		if (ImGui::Button("Refresh", ImVec2(100, 20)))
		{
			FindAllDemos();
		}

		for (const auto& fullDemoPath : discoveredDemoPaths)
		{
			auto relativeDemoPath = fullDemoPath.string().substr(PathUtils::GetCurrentGameDirectory().size());
			ImGui::Text("%s", relativeDemoPath.c_str());
			ImGui::SameLine(500);

			if (ImGui::Button(std::format("Load##{0}", relativeDemoPath).c_str(), ImVec2(60, 20)))
			{
				Mod::GetGameInterface()->PlayDemo(fullDemoPath);
			}
		}

		ImGui::End();
	}

	void DemoLoader::Release()
	{}
}