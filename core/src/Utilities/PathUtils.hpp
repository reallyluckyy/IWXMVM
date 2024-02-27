#pragma once

namespace IWXMVM::PathUtils
{
    std::string GetCurrentExecutablePath();
    std::string GetCurrentGameDirectory();
    std::filesystem::path GetIWXMVMPath();
    std::optional<std::filesystem::path> OpenFileDialog(bool saveDialog, DWORD flags, const char* filterString,
                                                        const char* extension, 
                                                        std::optional<std::filesystem::path> initialDir = std::nullopt);
}  // namespace IWXMVM::PathUtils