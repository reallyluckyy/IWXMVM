#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
    class DemoLoader : public UIComponent
    {
       public:
        void Render() final;
        void Release() final;

       private:
        struct DemoDirectory
        {
            std::filesystem::path path;  // Path
            std::pair<std::size_t, std::size_t>
                subdirectories;  // Pair of indices representing the [first, second) interval of directories in the
                                 // 'demoDirectories' vector
            std::pair<std::size_t, std::size_t>
                demos;  // Pair of indices representing the [first, second) interval of demos in the 'demoPaths' vector
            std::optional<std::size_t> parentIdx;  // Parent directory's index in the 'demoDirectories' vector. Search
                                                   // paths will contain a nullopt value
            bool relevant;                         // Does this directory ever reach a demo down the line?
        };

        void Initialize() final;
        void AddPathsToSearch(const std::vector<std::filesystem::path>& dirs);
        void SearchDir(std::size_t dirIdx);  // Recursive search function
        void Search();
        void MarkDirsRelevancy();
        void FindAllDemos();

        void RenderDemos(const std::pair<std::size_t, std::size_t>& demos);
        void RenderDir(const DemoDirectory& dir);  // Recursive render function
        void RenderSearchPaths();
        std::wstring stringToWide(const std::string& str);

        std::pair<std::size_t, std::size_t> searchPaths;  // Pair of indices representing the [first, second) interval
                                                          // of search paths in the 'demoDirectories' vector
        std::vector<DemoDirectory> demoDirectories;
        std::vector<std::filesystem::path> demoPaths;
        std::atomic<bool> isScanningDemoPaths;
    };
}  // namespace IWXMVM::UI
