#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class DemoLoader : public UIComponent
	{
	public:
		DemoLoader()
		{
			Initialize();
		}
		~DemoLoader()
		{
			Release();
		}

		void Render() final;
		void Release() final;

	private:
		struct DemoDirectory
		{
			std::filesystem::path dirPath;
			std::size_t demoPathsVecIdx = 0;
			std::size_t demoCount = 0;
			std::size_t directoriesVecIdx = 0;
			std::size_t subdirsCount = 0;
		};

		void Initialize() final;
		bool CheckFileForDemo(const std::filesystem::path& file, std::string_view demoExtension);
		bool SearchDir(const std::filesystem::path& dir);
		void FindAllDemos();
		void RenderDemosInDirectory(const DemoDirectory& directory);
		void SkipDirectories(const DemoDirectory& directory);
		void RenderDirectory(const DemoDirectory& directory);

		std::vector<std::size_t> searchPaths;
		std::vector<DemoDirectory> demoDirectories;
		std::size_t demoDirectoriesIterator = 0;
		std::vector<std::filesystem::path> demoPaths;
		bool initiallyLoadedDemos = false;
		std::atomic<bool> isScanningDemoPaths;
	};
}