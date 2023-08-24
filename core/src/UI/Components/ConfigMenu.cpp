#include "StdInclude.hpp"
#include "ConfigMenu.h"

#include "Utilities/HookManager.hpp"
#include "UI/UIManager.hpp"
#include "Mod.hpp"
#include <Windows.h>
#include <shobjidl.h> 

namespace IWXMVM::UI
{

	void ConfigMenu::Initialize()
	{
	}

	void ConfigMenu::Render()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGui::Begin("Config", nullptr, ImGuiWindowFlags_NoScrollbar);

        // '##___' is required to make a blank label, otherwise issues arise with ImGui not being able to ID the textbox.
		ImGui::InputTextMultiline("##BLANK", this->textBuffer, TEXT_BUFFER_SIZE, ImVec2(380, 200));

		if (ImGui::Button("Load CFG", ImVec2(90, 25)))
		{
            std::string fileContent = LoadTextFromFile();
            if (fileContent.length() < sizeof(textBuffer)) {
                strcpy_s(textBuffer, sizeof(textBuffer), fileContent.c_str());
            }
            else {
                LOG_ERROR("File content is too large for textBuffer");
            }
		}

		ImGui::SameLine();
        
        if (ImGui::Button("Send CFG", ImVec2(90, 25)))
        {
            std::string config;
            config += textBuffer;
            Mod::GetGameInterface()->ExecCommand(config);
        }
		
        ImGui::End();
	}

	void ConfigMenu::Release()
	{}

    std::string ConfigMenu::LoadTextFromFile()
    {
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        std::string output;
        if (SUCCEEDED(hr)) {
            IFileOpenDialog* pFileOpen;
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr)) {
                hr = pFileOpen->Show(NULL);
                if (SUCCEEDED(hr)) {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR filePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                        if (SUCCEEDED(hr)) {
                            char filePathA[MAX_PATH];
                            size_t convertedChars = 0;
                            wcstombs_s(&convertedChars, filePathA, MAX_PATH, filePath, _TRUNCATE);

                            std::ifstream fileStream(filePathA);
                            std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
                            output = fileContent;

                            CoTaskMemFree(filePath);
                        }
                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }

        return output;
    }
}