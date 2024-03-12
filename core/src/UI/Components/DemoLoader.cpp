#include "StdInclude.hpp"
#include "DemoLoader.hpp"

#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Resources.hpp"

namespace IWXMVM::UI
{
    template <bool caseSensitive, typename T>
        requires std::is_same_v<T, std::string_view> || std::is_same_v<T, std::wstring_view>
    bool CompareNaturally(const T lhs, const T rhs)
    {
        auto IsDigit = [](auto c) { return std::iswdigit(c); };

        auto ToUpper = [](auto c) {
            if constexpr (caseSensitive)
                return c;
            else
                return std::towupper(c);
        };

        auto StringToUint = [](const auto* str, auto* output, std::size_t* endPtr = nullptr) {
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

                // when sorting a container, move the 'unparsable' string_view to end of container if string-to-uint
                // throws an exception
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
            return CompareNaturally<false>(StringView{lhsFileNamePtr, lhsSvLength},
                                           StringView{rhsFileNamePtr, rhsSvLength});
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
            return CompareNaturally<false>(StringView{lhsDirPtr, lhsSvLength}, StringView{rhsDirNamePtr, rhsSvLength});
        });
    }

    bool IsFileDemo(const std::filesystem::path& file)
    {
        return file.extension().compare(Mod::GetGameInterface()->GetDemoExtension())
                   ? false
                   : true;  // .compare() == 0 => strings are equal
    }

    void DemoLoader::AddPathsToSearch(const std::vector<std::filesystem::path>& dirs)
    {
        for (const auto& dir : dirs)
        {
            if (std::filesystem::exists(dir))
            {
                DemoDirectory searchPath = {.path = dir};
                demoDirectories.push_back(searchPath);
            }
        }
        searchPaths = std::make_pair(0, demoDirectories.size());
    }

    void DemoLoader::SearchDir(std::size_t dirIdx)
    {
        auto tempDir = std::string(DEMO_TEMP_DIRECTORY);
        if (demoDirectories[dirIdx].path.wstring().find(std::wstring(tempDir.begin(), tempDir.end())) != std::string::npos)
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
                DemoDirectory subdir = {.path = entry.path(), .parentIdx = dirIdx};
                demoDirectories.push_back(subdir);
            }
            else if (IsFileDemo(entry.path()))
            {
                demoPaths.push_back(entry.path());
            }
        }

        SortDemoPaths(std::span{(demoPaths.begin() + demosStartIdx), demoPaths.end()});
        SortDemoDirectories(std::span{(demoDirectories.begin() + subdirsStartIdx), demoDirectories.end()},
                            [](const DemoDirectory& data) -> const std::filesystem::path& { return data.path; });

        demoDirectories[dirIdx].demos = std::make_pair(demosStartIdx, demoPaths.size());
        if (demoDirectories[dirIdx].demos.first != demoDirectories[dirIdx].demos.second)
        {
            demoDirectories[dirIdx].relevant = true;
        }

        demoDirectories[dirIdx].subdirectories = std::make_pair(subdirsStartIdx, demoDirectories.size());
        for (auto i = demoDirectories[dirIdx].subdirectories.first; i < demoDirectories[dirIdx].subdirectories.second;
             i++)
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

        AddPathsToSearch({PathUtils::GetCurrentGameDirectory()});
        Search();

        // 'demoDirectories' and 'demoPaths' are complete here, but we still need to find out the relevancy of each
        // directory
        MarkDirsRelevancy();

        isScanningDemoPaths.store(false);
    }

    static bool DemoFilter(const std::u8string demoFileName, const std::u8string searchBarText)
    {
        //return true to keep
        return demoFileName.find(searchBarText) != std::string::npos;
    }

    void DemoLoader::RefreshFilteredDemosMask()
    {
        LOG_DEBUG("Refreshing filteredDemosMask");
        filteredDemosMask.resize(demoPaths.size());
        for (size_t i = 0; i < demoPaths.size(); ++i)
        {
            try
            {
                if (searchBarText.empty())
                {
                    filteredDemosMask[i] = true;
                }
                else
                {
                    filteredDemosMask[i] = DemoFilter(demoPaths[i].filename().u8string(),
                                                      std::u8string(searchBarText.begin(), searchBarText.end()));
                }
            }
            catch (std::exception&)
            {
                LOG_ERROR("Error filtering demoPath");
                // catching errors just in case
            }
        }
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
                try
                {
                    auto demoName = demoPaths[idx].filename().string();

                    if (Mod::GetGameInterface()->GetGameState() == Types::GameState::InDemo &&
                        Mod::GetGameInterface()->GetDemoInfo().name == demoName)
                    {
                        if (ImGui::Button(std::format(ICON_FA_STOP " STOP##{0}", demoName).c_str(),
                                          ImVec2(ImGui::GetFontSize() * 4, ImGui::GetFontSize() * 1.5f)))
                        {
                            Mod::GetGameInterface()->Disconnect();
                        }
                    }
                    else
                    {
                        if (ImGui::Button(std::format(ICON_FA_PLAY " PLAY##{0}", demoName).c_str(),
                                          ImVec2(ImGui::GetFontSize() * 4, ImGui::GetFontSize() * 1.5f)))
                        {
                            Mod::GetGameInterface()->PlayDemo(demoPaths[idx]);
                        }
                    }

                    ImGui::SameLine();

                    ImGui::Text("%s", demoName.c_str());
                }
                catch (std::exception&)
                {
                    ImGui::BeginDisabled();
                    ImGui::Button(ICON_FA_TRIANGLE_EXCLAMATION " ERROR");
                    ImGui::SameLine();
                    ImGui::Text("<invalid demo name>");
                    ImGui::EndDisabled();
                }
            }
        }
    }

    void DemoLoader::FilteredRenderDemos(const std::pair<std::size_t, std::size_t>& demos)
    {
        std::size_t startIdx = demos.first;
        std::size_t endIdx = demos.first;

        for (std::size_t i = demos.first; i < demos.second; i++)
        {
            if (filteredDemosMask[i])
            {
                startIdx = i;
                do
                {
                    i++;
                } while (i < demos.second && filteredDemosMask[i]);
                endIdx = i;
                RenderDemos({ startIdx, endIdx });
            }    
        }
    }


    void DemoLoader::RenderDir(const DemoDirectory& dir)
    {
        try
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

                FilteredRenderDemos(dir.demos);

                ImGui::TreePop();
            }
        }
        catch (std::exception&)
        {
			ImGui::BeginDisabled();
			ImGui::TreeNode("<invalid directory name>");
            ImGui::TreePop();
			ImGui::EndDisabled();
		}
    }

    void DemoLoader::RenderSearchBar()
    {
        ImGui::InputText(
            "Search", &searchBarText[0], searchBarText.capacity() + 1, ImGuiInputTextFlags_CallbackResize,
            [](ImGuiInputTextCallbackData* data) -> int {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    std::string* str = (std::string*)data->UserData;
                    str->resize(data->BufTextLen);
                    data->Buf = &(*str)[0];
                }
                return 0;
            },
            &searchBarText);

        if (lastSearchBarText != searchBarText)
        {
            RefreshFilteredDemosMask();
            lastSearchBarText = searchBarText;
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
                    for (auto j = demoDirectories[i].subdirectories.first; j < demoDirectories[i].subdirectories.second;
                         j++)
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

                FilteredRenderDemos(demoDirectories[i].demos);

                ImGui::TreePop();
            }
        }
    }

    void DemoLoader::Initialize()
    {
        isScanningDemoPaths.store(true);
        std::thread([&] { FindAllDemos(); }).detach();
    }

    void DemoLoader::Render()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Demos", nullptr, flags))
        {
            ImGui::AlignTextToFramePadding();

            if (isScanningDemoPaths.load())
            {
                ImGui::Text("Searching for demo files...");
            }
            else
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%d demos found!", demoPaths.size());
                ImGui::SameLine();

                auto addPathButtonLabel = std::string(ICON_FA_FOLDER_OPEN " Add path");
                auto refreshButtonLabel = std::string(ICON_FA_ROTATE_RIGHT " Refresh");
                auto buttonSize = ImVec2(ImGui::GetFontSize() * 6.0f, ImGui::GetFontSize() * 1.75f);

                if (ImGui::GetWindowWidth() < buttonSize.x * 4)
                {
                    buttonSize = ImVec2(buttonSize.y, buttonSize.y);
                    addPathButtonLabel = addPathButtonLabel.substr(0, addPathButtonLabel.find(" "));
                    refreshButtonLabel = refreshButtonLabel.substr(0, refreshButtonLabel.find(" "));
                }

                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonSize.x * 2 - ImGui::GetStyle().ItemSpacing.x -
                                     ImGui::GetStyle().WindowPadding.x);

                if (ImGui::Button(addPathButtonLabel.c_str(), buttonSize))
                {
                    // TODO: Implement
                }

                ImGui::SameLine();

                if (ImGui::Button(refreshButtonLabel.c_str(), buttonSize))
                {
                    isScanningDemoPaths.store(true);
                    std::thread([&] { FindAllDemos(); }).detach();
                    ImGui::End();
                    return;
                }

                // Search paths will always be rendered, even if empty
                RenderSearchBar();
                RenderSearchPaths();
            }

            ImGui::End();
        }
    }

    void DemoLoader::Release()
    {
    }
}  // namespace IWXMVM::UI
