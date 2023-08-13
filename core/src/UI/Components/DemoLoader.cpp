#include "StdInclude.hpp"
#include "DemoLoader.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"
#include "CustomImGuiControls.hpp"

namespace IWXMVM::UI
{
	bool DemoLoader::CheckFileForDemo(const std::filesystem::path& file, std::string_view demoExtension)
	{
		if (file.extension().compare(demoExtension) == 0)
		{
			demoPaths.push_back(file);
			return true;
		}
		return false;
	}

	void DemoLoader::SearchDir(const std::filesystem::path& dir)
	{
		DemoDirectory demoDir = { .demoPathsVecIdx = demoPaths.size() };

		for (std::filesystem::directory_iterator i(dir), end; i != end; ++i)
		{
			if (i->is_directory())
			{
				if (i->path().filename().string().compare(Mod::GetGameInterface()->GetTempDemoDirName()) != 0)
				{
					SearchDir(i->path());
				}
			}
			else if (CheckFileForDemo(i->path(), Mod::GetGameInterface()->GetDemoExtension()))
			{
				demoDir.demoCount++;
			}
		}

		if (demoDir.demoCount > 0)
		{
			demoDir.dirPath = dir;
			demoDirectories.push_back(demoDir);
		}
	}

	void DemoLoader::FindAllDemos()
	{
		isScanningDemoPaths.store(true);

		demoPaths.clear();
		demoDirectories.clear();

		for (auto& searchPath : searchPaths)
		{
			searchPath.demoCount = 0;
			searchPath.demoPathsVecIdx = demoPaths.size();

			for (std::filesystem::directory_iterator i(searchPath.dirPath), end; i != end; ++i)
			{
				if (i->is_directory())
				{
					SearchDir(i->path());
				}
				else if (CheckFileForDemo(i->path(), Mod::GetGameInterface()->GetDemoExtension()))
				{
					searchPath.demoCount++;
				}
			}
		}

		isScanningDemoPaths.store(false);
	}

	void DemoLoader::Initialize()
	{
		searchPaths = { { .dirPath = PathUtils::GetCurrentGameDirectory() } };
	}

	void DemoLoader::RenderDemosInDirectory(const DemoDirectory& directory)
	{
		for (auto i = directory.demoPathsVecIdx; i < directory.demoPathsVecIdx + directory.demoCount; ++i)
		{
			auto demoName = demoPaths[i].filename().string();

			if (ImGui::Button(std::format("Load##{0}", demoName).c_str(), ImVec2(60, 20)))
			{
				Mod::GetGameInterface()->PlayDemo(demoPaths[i]);
			}

			ImGui::SameLine();

			ImGui::Text("%s", demoName.c_str());
		}
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
			ImGui::Text("%d demos found!", demoPaths.size());
			ImGui::SameLine();
			if (ImGui::Button("Refresh", ImVec2(100, 20)))
			{
				isScanningDemoPaths.store(true);
				std::thread([&] { FindAllDemos(); }).detach();
				ImGui::End();
				return;
			}

			for (const auto& searchPath : searchPaths)
			{
				if (ImGui::TreeNode(searchPath.dirPath.string().c_str()))
				{
					for (const auto& directory : demoDirectories)
					{
						auto directoryLabel = directory.dirPath.string().substr(searchPath.dirPath.string().size() + 1);
						if (ImGui::TreeNode(directoryLabel.c_str()))
						{
							RenderDemosInDirectory(directory);

							ImGui::TreePop();
						}
					}

					if (searchPath.demoCount > 0)
					{
						RenderDemosInDirectory(searchPath);
					}

					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}

	void DemoLoader::Release()
	{}
}