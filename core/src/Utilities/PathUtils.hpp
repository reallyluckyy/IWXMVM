#pragma once

namespace IWXMVM::PathUtils
{
	ImVec2 GetWindowSize(HWND windowHandle);
	std::string GetCurrentExecutablePath();
	std::string GetCurrentGameDirectory();
}
