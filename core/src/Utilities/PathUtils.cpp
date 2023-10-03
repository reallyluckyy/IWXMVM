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
}  // namespace IWXMVM::PathUtils