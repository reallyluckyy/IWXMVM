#include "StdInclude.hpp"
#include "DemoLoader.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{

	void DemoLoader::FindAllDemos()
	{
		isScanningDemoPaths.store(true);

		discoveredDemoPaths.clear();

		for (std::filesystem::recursive_directory_iterator i(PathUtils::GetCurrentGameDirectory()), end; i != end; ++i)
		{
			auto demoExtensions = Mod::GetGameInterface()->GetDemoExtensions();

			if (!std::filesystem::is_directory(i->path()) 
				&& i->path().has_extension() 
				&& i->path().string().find(DEMO_TEMP_DIR_NAME) == std::string::npos
				&& std::find(demoExtensions.begin(), demoExtensions.end(), i->path().extension().string()) != demoExtensions.end())
			{
				discoveredDemoPaths.push_back(i->path());
			}
		}

		isScanningDemoPaths.store(false);
	}

	void DemoLoader::Initialize()
	{
	}

	void DemoLoader::Render()
	{
		if (!initiallyLoadedDemos) 
		{
			// would be cool to be able to do this in Initialize() at some point
			// but with the current set up we still dont have access to Mod::GetGameInterface() at that time
			isScanningDemoPaths.store(true);
			std::thread([&] { FindAllDemos(); }).detach();
			initiallyLoadedDemos = true;
		}

		ImGui::Begin("Demos", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

		if (isScanningDemoPaths.load())
		{
			ImGui::Text("Searching for demo files...");
		}
		else
		{
			ImGui::Text("%d demos found!", discoveredDemoPaths.size());
			ImGui::SameLine();
			if (ImGui::Button("Refresh", ImVec2(100, 20)))
			{
				isScanningDemoPaths.store(true);
				std::thread([&] { FindAllDemos(); }).detach();
				ImGui::End();
				return;
			}

			for (const auto& fullDemoPath : discoveredDemoPaths)
			{
				auto relativeDemoPath = fullDemoPath.string().substr(PathUtils::GetCurrentGameDirectory().size() + 1);

				if (ImGui::Button(std::format("Load##{0}", relativeDemoPath).c_str(), ImVec2(60, 20)))
				{
					Mod::GetGameInterface()->PlayDemo(fullDemoPath);
				}

				ImGui::SameLine();

				ImGui::Text("%s", relativeDemoPath.c_str());
			}
		}

		ImGui::End();
	}

	void DemoLoader::Release()
	{}
}