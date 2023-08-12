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
		void Initialize() final;
		void FindAllDemos();

		std::vector<std::filesystem::path> searchPaths;
		std::set<std::filesystem::path> foundDemoDirectories;
		std::vector<std::filesystem::path> discoveredDemoPaths;
		bool initiallyLoadedDemos = false;
		std::atomic<bool> isScanningDemoPaths;
	};
}