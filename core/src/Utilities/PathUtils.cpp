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

    // TODO: Unicode directories will break this
    std::optional<std::filesystem::path> OpenFolderBrowseDialog()
    {
        std::optional<std::filesystem::path> result = std::nullopt;

        // Create FileOpenDialog object
        IFileOpenDialog* pFileOpen = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog,
                                      reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Set options to allow only folder selection
            FILEOPENDIALOGOPTIONS options = {};
            hr = pFileOpen->GetOptions(&options);
            hr = pFileOpen->SetOptions(options | FOS_PICKFOLDERS);

            // Convert path to wchar string
            const std::string charPath = PathUtils::GetCurrentGameDirectory();
            const std::size_t convertedChars = mbstowcs(nullptr, charPath.c_str(), charPath.length());
            wchar_t* wcharPath = new wchar_t[convertedChars + 1];
            mbstowcs(wcharPath, charPath.c_str(), convertedChars + 1);

            // Set an initial folder
            IShellItem* pInitialFolder = nullptr;
            if (wcharPath)
            {
                hr = SHCreateItemFromParsingName(wcharPath, nullptr, IID_PPV_ARGS(&pInitialFolder));
                if (SUCCEEDED(hr))
                {
                    pFileOpen->SetFolder(pInitialFolder);
                    pInitialFolder->Release();
                }
            }
            delete[] wcharPath;

            // Show the dialog
            hr = pFileOpen->Show(NULL);

            // Get the selected folder path
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem = nullptr;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath = nullptr;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr))
                    {
                        result = std::filesystem::path(pszFilePath);
                    }

                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }

        return result;
    }
}  // namespace IWXMVM::PathUtils