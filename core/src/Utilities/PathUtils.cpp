#include "StdInclude.hpp"
#include "PathUtils.hpp"

namespace IWXMVM::PathUtils
{
	// TODO: remove this and get resolution from game memory
	ImVec2 GetWindowSize(HWND windowHandle)
	{
		RECT rect;
		if (GetWindowRect(windowHandle, &rect))
		{
			return ImVec2(rect.right - rect.left, rect.bottom - rect.top);
		}

		return ImVec2();
	}

	std::string GetCurrentExecutablePath()
	{
		char buffer[MAX_PATH];
		auto result = GetModuleFileName(NULL, buffer, MAX_PATH);
		if (!result)
		{
			LOG_ERROR("GetModuleFileName failed with result {}", result);
		}

		return buffer;
	}

	std::string GetCurrentGameDirectory()
	{
		std::filesystem::path executablePath = GetCurrentExecutablePath();
		return executablePath.parent_path().string();
	}
}