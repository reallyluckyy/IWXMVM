#include "StdInclude.hpp"
#include "DemoLoader.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"
#include "CustomImGuiControls.hpp"

namespace IWXMVM::UI
{
	template <bool caseSensitive, typename T>
		requires std::is_same_v<T, std::string_view> || std::is_same_v<T, std::wstring_view>
	bool CompareNaturally(const T lhs, const T rhs)
	{
		auto IsDigit = [](auto c) 
		{
			return std::iswdigit(c);
		};
	
		auto ToUpper = [](auto c) 
		{
			if constexpr (caseSensitive) 
				return c;
			else 
				return std::towupper(c);
		};
	
		auto StringToUint = [](const auto* str, auto* output, std::size_t* endPtr = nullptr)
		{
			try
			{
				*output = std::stoull(str, endPtr);
				return true;
			}
			catch (...)
			{
				return false;
			}
		};
	
		for (auto lhsItr = lhs.begin(), rhsItr = rhs.begin(); lhsItr != lhs.end() && rhsItr != rhs.end();)
		{
			if (IsDigit(*lhsItr) && IsDigit(*rhsItr)) 
			{
				std::uint64_t lhsNum = 0;
				std::uint64_t rhsNum = 0;
				std::size_t lhsDigitCount = 0;
				std::size_t rhsDigitCount = 0;
	
				// when sorting a container, move the 'unparsable' string_view to end of container if string-to-uint throws an exception
				if (!StringToUint(std::addressof(*lhsItr), &lhsNum, &lhsDigitCount)) 
					return false;
				if (!StringToUint(std::addressof(*rhsItr), &rhsNum, &rhsDigitCount))
					return true;
	
				if (lhsNum != rhsNum) 
					return lhsNum < rhsNum;
	
				assert(lhsDigitCount == std::distance(lhsItr, std::find_if_not(lhsItr, lhs.end(), IsDigit)));
				assert(rhsDigitCount == std::distance(rhsItr, std::find_if_not(rhsItr, rhs.end(), IsDigit)));
	
				lhsItr += lhsDigitCount;
				rhsItr += rhsDigitCount;
			} 
			else 
			{
				if (ToUpper(*lhsItr) != ToUpper(*rhsItr)) 
					return *lhsItr < *rhsItr;
	
				++lhsItr;
				++rhsItr;
			}
		}
	
		return lhs.length() < rhs.length();
	}
	
	void SortDemoPaths(const auto demos)
	{
		std::sort(demos.begin(), demos.end(), [&](const auto& lhs, const auto& rhs) {
			const std::size_t extLength = Mod::GetGameInterface()->GetDemoExtension().length();
			const std::size_t dirLength = demos.front().parent_path().native().length();
			const std::size_t lhsLength = lhs.native().length();
			const std::size_t rhsLength = rhs.native().length();
	
			assert(lhsLength > dirLength + extLength + 1 && rhsLength > dirLength + extLength + 1);
	
			const auto* lhsFileNamePtr = lhs.c_str() + dirLength + 1;
			const auto* rhsFileNamePtr = rhs.c_str() + dirLength + 1;
			const std::size_t lhsSvLength = lhsLength - dirLength - extLength - 1;
			const std::size_t rhsSvLength = rhsLength - dirLength - extLength - 1;
	
			using StringView = std::wstring_view;
			return CompareNaturally<false>(StringView{ lhsFileNamePtr, lhsSvLength }, StringView{ rhsFileNamePtr, rhsSvLength });
		});
	}
	
	void SortDemoDirectories(const auto directories, auto GetPath)
	{
		std::sort(directories.begin(), directories.end(), [&](const auto& lhs, const auto& rhs) {
			const auto& lhsPath = GetPath(lhs);
			const auto& rhsPath = GetPath(rhs);
	
			const std::size_t parentDirLength = GetPath(directories.front()).parent_path().native().length();
			const std::size_t lhsLength = lhsPath.native().length();
			const std::size_t rhsLength = rhsPath.native().length();
	
			assert(lhsLength > parentDirLength + 1 && rhsLength > parentDirLength + 1);
	
			const auto* lhsDirPtr = lhsPath.c_str() + parentDirLength + 1;
			const auto* rhsDirNamePtr = rhsPath.c_str() + parentDirLength + 1;
			const std::size_t lhsSvLength = lhsLength - parentDirLength - 1;
			const std::size_t rhsSvLength = rhsLength - parentDirLength - 1;
	
			using StringView = std::wstring_view;
			return CompareNaturally<false>(StringView{ lhsDirPtr, lhsSvLength }, StringView{ rhsDirNamePtr, rhsSvLength });
		});
	}

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
					.path = dir
				};
				demoDirectories.push_back(searchPath);
			}
		}
		searchPaths = std::make_pair(0, demoDirectories.size());
	}

	void DemoLoader::SearchDir(std::size_t dirIdx)
	{
		if (demoDirectories[dirIdx].path.string().find(DEMO_TEMP_DIRECTORY) != std::string::npos)
		{
			demoDirectories[dirIdx].relevant = false;
			return;
		}

		auto subdirsStartIdx = demoDirectories.size();
		auto demosStartIdx = demoPaths.size();

		for (const auto& entry : std::filesystem::directory_iterator(demoDirectories[dirIdx].path))
		{
			if (entry.is_directory())
			{
				DemoDirectory subdir = {
					.path = entry.path(),
					.parentIdx = dirIdx
				};
				demoDirectories.push_back(subdir);
			}
			else if (IsFileDemo(entry.path()))
			{
				demoPaths.push_back(entry.path());
			}
		}

		SortDemoPaths(std::span{ (demoPaths.begin() + demosStartIdx), demoPaths.end() });
		SortDemoDirectories(
			std::span{ (demoDirectories.begin() + subdirsStartIdx), demoDirectories.end() },
			[](const DemoDirectory& data) -> const std::filesystem::path& { return data.path; }
		);
		
		demoDirectories[dirIdx].demos = std::make_pair(demosStartIdx, demoPaths.size());
		if (demoDirectories[dirIdx].demos.first != demoDirectories[dirIdx].demos.second)
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

	void DemoLoader::MarkDirsRelevancy()
	{
	    for (auto it = demoDirectories.rbegin(); it != demoDirectories.rend(); it++)
	    {
	        std::size_t vecIdx = std::abs(it - demoDirectories.rend() + 1);
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
		MarkDirsRelevancy();

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
