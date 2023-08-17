#include "StdInclude.hpp"
#include "DemoLoader.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"
#include "CustomImGuiControls.hpp"

namespace IWXMVM::UI
{
	void DemoLoader::Initialize()
	{
	}

	bool IsFileDemo(const std::filesystem::path& file)
	{
		return file.extension().compare(Mod::GetGameInterface()->GetDemoExtension()) ? false : true; // .compare() == 0 => strings are equal
	}

	void DemoLoader::AddPathsToSearch(const std::vector<std::filesystem::path>& dirs)
	{
		for (const auto& dir : dirs)
		{
			if (std::filesystem::exists(dir))
			{
				DemoDirectory searchPath = {
					.path = dir,
					.idx = demoDirectories.size(),
				};
				demoDirectories.push_back(searchPath);
			}
		}
		searchPaths = std::make_pair(0, demoDirectories.size());
	}

	void DemoLoader::SearchDir(std::size_t dirIdx)
	{
		auto subdirsStartIdx = demoDirectories.size();
		auto demosStartIdx = demoPaths.size();

		for (const auto& entry : std::filesystem::directory_iterator(demoDirectories[dirIdx].path))
		{
			if (entry.is_directory())
			{
				DemoDirectory subdir = {
					.path = entry.path(),
					.idx = demoDirectories.size(),
					.parentIdx = demoDirectories[dirIdx].idx
				};
				demoDirectories.push_back(subdir);
			}
			else if (IsFileDemo(entry.path()))
			{
				demoPaths.push_back(entry.path());
			}
		}

		demoDirectories[dirIdx].demos = std::make_pair(demosStartIdx, demoPaths.size());
		if (demoDirectories[dirIdx].demos.first != demoDirectories[dirIdx].demos.second
			&& demoDirectories[dirIdx].path.filename().compare(Mod::GetGameInterface()->GetTempDemoDirName()) != 0)
		{
			demoDirectories[dirIdx].relevant = true;
		}

		demoDirectories[dirIdx].subdirectories = std::make_pair(subdirsStartIdx, demoDirectories.size());
		for (auto i = demoDirectories[dirIdx].subdirectories.first; i < demoDirectories[dirIdx].subdirectories.second; i++)
		{
			SearchDir(i);
		}
	}

	void DemoLoader::Search()
	{
		for (auto i = searchPaths.first; i < searchPaths.second; i++)
		{
			SearchDir(i);
		}
	}

	void DemoLoader::SpreadDirsRelevancy()
	{
		for (auto it = demoDirectories.rbegin(); it != demoDirectories.rend(); it++)
		{
			auto vecIdx = it->idx;
			if (demoDirectories[vecIdx].relevant)
			{
				while (demoDirectories[vecIdx].parentIdx.has_value())
				{
					vecIdx = demoDirectories[vecIdx].parentIdx.value();
					if (demoDirectories[vecIdx].relevant)
					{
						break;
					}
					demoDirectories[vecIdx].relevant = true;
				}
			}
		}
	}

	void DemoLoader::FindAllDemos()
	{
		isScanningDemoPaths.store(true);

		demoDirectories.clear();
		demoPaths.clear();

		AddPathsToSearch({ PathUtils::GetCurrentGameDirectory() });
		Search();

		// 'demoDirectories' and 'demoPaths' are complete here, but we still need to find out the relevancy of each directory
		SpreadDirsRelevancy();

		isScanningDemoPaths.store(false);
	}

	void DemoLoader::RenderDemos(const std::pair<std::size_t, std::size_t>& demos)
	{
		ImGuiListClipper clipper;
		clipper.Begin(demos.second - demos.first);

		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				auto idx = i + demos.first;
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

	void DemoLoader::RenderDir(const DemoDirectory& dir)
	{
		if (ImGui::TreeNode(dir.path.filename().string().c_str()))
		{
			for (auto i = dir.subdirectories.first; i < dir.subdirectories.second; i++)
			{
				if (demoDirectories[i].relevant)
				{
					RenderDir(demoDirectories[i]);
				}
			}

			RenderDemos(dir.demos);

			ImGui::TreePop();
		}
	}

	void DemoLoader::RenderSearchPaths()
	{
		for (auto i = searchPaths.first; i < searchPaths.second; i++)
		{
			if (ImGui::TreeNode(demoDirectories[i].path.string().c_str()))
			{
				if (demoDirectories[i].relevant)
				{
					for (auto j = demoDirectories[i].subdirectories.first; j < demoDirectories[i].subdirectories.second; j++)
					{
						if (demoDirectories[j].relevant)
						{
							RenderDir(demoDirectories[j]);
						}
					}
				}
				else
				{
					ImGui::Text("Search path is empty.");
				}

				RenderDemos(demoDirectories[i].demos);

				ImGui::TreePop();
			}
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

			// Search paths will always be rendered, even if empty
			RenderSearchPaths();
		}

		ImGui::End();
	}

	void DemoLoader::Release()
	{}
}