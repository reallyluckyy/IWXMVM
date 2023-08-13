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

	bool DemoLoader::SearchDir(const std::filesystem::path& dir)
	{
		DemoDirectory demoDir = { .dirPath = dir, .demoPathsVecIdx = demoPaths.size() };
		bool isDirectoryRelevant = false;

		for (std::filesystem::directory_iterator i(dir), end; i != end; ++i)
		{
			if (CheckFileForDemo(i->path(), Mod::GetGameInterface()->GetDemoExtension()))
			{
				demoDir.demoCount++;
				isDirectoryRelevant = true;
			}
		}

		for (std::filesystem::directory_iterator i(dir), end; i != end; ++i)
		{
			if (i->is_directory() && i->path().filename().string().compare(Mod::GetGameInterface()->GetTempDemoDirName()) != 0)
			{
				if (SearchDir(i->path()))
				{
					demoDir.subdirsCount++;
					isDirectoryRelevant = true;
				}
			}
		}

		if (isDirectoryRelevant)
		{
			demoDir.directoriesVecIdx = demoDirectories.size();
			demoDirectories.push_back(demoDir);

			return true;
		}
		else
		{
			return false;
		}
	}

	void DemoLoader::FindAllDemos()
	{
		isScanningDemoPaths.store(true);

		searchPaths.clear();
		demoPaths.clear();
		demoDirectories.clear();

		if (SearchDir(PathUtils::GetCurrentGameDirectory()))
		{
			searchPaths.push_back(demoDirectories.size() - 1);
		}

		isScanningDemoPaths.store(false);
	}

	void DemoLoader::Initialize()
	{
	}

	void DemoLoader::RenderDemosInDirectory(const DemoDirectory& directory)
	{
		ImGuiListClipper clipper;
		clipper.Begin(directory.demoCount);
		
		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
			{
				auto idx = i + directory.demoPathsVecIdx;

				auto demoName = demoPaths[idx].filename().string();

				if (ImGui::Button(std::format("Load##{0}", demoName).c_str(), ImVec2(60, 20)))
				{
					Mod::GetGameInterface()->PlayDemo(demoPaths[idx]);
				}

				ImGui::SameLine();

				ImGui::Text("%s", demoName.c_str());
			}
		}
	}

	void DemoLoader::SkipDirectories(const DemoDirectory& directory)
	{
		for (std::size_t i = 0; i < directory.subdirsCount; i++)
		{
			demoDirectoriesIterator--;
			SkipDirectories(demoDirectories[demoDirectoriesIterator]);
		}
	}

	void DemoLoader::RenderDirectory(const DemoDirectory& directory)
	{
		if (ImGui::TreeNode(directory.dirPath.string().c_str() + directory.dirPath.string().find_last_of('\\') + 1))
		{
			for (std::size_t i = 0; i < directory.subdirsCount; i++)
			{
				demoDirectoriesIterator--;
				RenderDirectory(demoDirectories[demoDirectoriesIterator]);
			}

			RenderDemosInDirectory(directory);
			ImGui::TreePop();
		}
		else
		{
			SkipDirectories(directory);
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
				demoDirectoriesIterator = demoDirectories[searchPath].directoriesVecIdx;
				RenderDirectory(demoDirectories[searchPath]);
			}
		}

		ImGui::End();
	}

	void DemoLoader::Release()
	{}
}