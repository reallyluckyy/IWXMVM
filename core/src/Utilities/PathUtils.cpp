#include "StdInclude.hpp"
#include "PathUtils.hpp"

namespace IWXMVM::PathUtils
{
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

    std::filesystem::path GetIWXMVMPath()
    {
        return std::filesystem::path(PathUtils::GetCurrentGameDirectory()) / "IWXMVM";
    }

    std::optional<std::filesystem::path> OpenFileDialog(bool saveDialog, DWORD flags, const char* filterString,
                                                        const char* extension,
                                                        std::optional<std::filesystem::path> initialDir)
    {
        CHAR szFile[2048] = {0};

        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filterString;
        ofn.nFilterIndex = 1;
        ofn.Flags = flags;
        ofn.lpstrDefExt = extension;
        if (initialDir.has_value())
        {
            ofn.lpstrInitialDir = initialDir.value().string().c_str();
        }

        BOOL result;
        if (saveDialog)
            result = GetSaveFileNameA(&ofn);
        else
            result = GetOpenFileNameA(&ofn);

        if (result == TRUE)
        {
            return std::filesystem::path(szFile);
        }
        else
        {
            return std::nullopt;
        }
    }
}  // namespace IWXMVM::PathUtils