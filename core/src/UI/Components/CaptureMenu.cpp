#include "StdInclude.hpp"
#include "CaptureMenu.hpp"

#include "Resources.hpp"
#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Components/CaptureManager.hpp"
#include "Components/CameraManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Configuration/PreferencesConfiguration.hpp"

namespace IWXMVM::UI
{
    void CaptureMenu::Initialize()
    {
    }

    void CaptureMenu::Render()
    {
        using namespace Components;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
        if (ImGui::Begin("Capture", NULL, flags))
        {
            if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            {
                UI::DrawInaccessibleTabWarning();
                ImGui::End();
                return;
            }

            auto& cameraManager = CameraManager::Get();
            auto& captureManager = CaptureManager::Get();
            auto& captureSettings = captureManager.GetCaptureSettings();

            const auto fieldLayoutPercentage = 0.4f;

            ImGui::BeginDisabled(captureManager.IsCapturing());

            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::Text("Capture Settings");
            ImGui::PopFont();

            auto halfWidth = ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) - ImGui::GetStyle().WindowPadding.x;
            halfWidth /= 2;
            halfWidth -= ImGui::GetStyle().ItemSpacing.x / 2;
            auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;

            ImGui::Checkbox("Custom Timeframe", &captureSettings.usingCustomTimeFrame);

            if (!captureSettings.usingCustomTimeFrame)
            {
                std::map<Types::KeyframeableProperty, std::vector<Types::Keyframe>> keyframes =
                    KeyframeManager::Get().GetKeyframes();

                if (!keyframes.empty())
                {
                    std::uint32_t minTick = UINT32_MAX;
                    std::uint32_t maxTick = 0;

                    for (const auto& pair : keyframes)
                    {
                        if (!pair.second.empty())
                        {
                            std::uint32_t front = pair.second.front().tick;
                            std::uint32_t back = pair.second.back().tick;
                            if (front < minTick)
                            {
                                minTick = front;
                            }
                            if (back > maxTick)
                            {
                                maxTick = back;
                            }
                        }
                    }

                    if (minTick != UINT32_MAX && maxTick && maxTick > minTick)
                    {
                        captureSettings.startTick = minTick;
                        captureSettings.endTick = maxTick;
                    }
                    else
                    {
                        captureManager.SetTimeFrameToDefault();
                    }
                }
                ImGui::BeginDisabled();
                
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Timeframe");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(halfWidth);
            ImGui::DragInt("##startTickInput", (int32_t*)&captureSettings.startTick, 10, 0, captureSettings.endTick);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(halfWidth);
            ImGui::DragInt("##endTickInput", (int32_t*)&captureSettings.endTick, 10, captureSettings.startTick,
                           endTick);
            if (!captureSettings.usingCustomTimeFrame)
            {
                ImGui::EndDisabled();
            }
            
            

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Output Format");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) -
                                    ImGui::GetStyle().WindowPadding.x);
            if (ImGui::BeginCombo("##captureMenuOutputFormatCombo",
                                  captureManager.GetOutputFormatLabel(captureSettings.outputFormat).data()))
            {
                for (auto outputFormat = 0; outputFormat < (int)OutputFormat::Count; outputFormat++)
                {
                    bool isSelected = captureSettings.outputFormat == (OutputFormat)outputFormat;
                    if (ImGui::Selectable(captureManager.GetOutputFormatLabel((OutputFormat)outputFormat).data(),
                            captureSettings.outputFormat == (OutputFormat)outputFormat))
                    {
                        captureSettings.outputFormat = (OutputFormat)outputFormat;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (captureSettings.outputFormat == OutputFormat::Video)
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Video Codec");
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) -
                                                        ImGui::GetStyle().WindowPadding.x);
                if (ImGui::BeginCombo("##captureMenuVideoCodecCombo",
                                        captureManager.GetVideoCodecLabel(captureSettings.videoCodec.value())
                                            .data()))
                {
                    for (auto videoCodec = 0; videoCodec < (int)VideoCodec::Count; videoCodec++)
                    {
                        bool isSelected = captureSettings.videoCodec == (VideoCodec)videoCodec;
                        if (ImGui::Selectable(captureManager.GetVideoCodecLabel((VideoCodec)videoCodec).data(),
                                captureSettings.videoCodec == (VideoCodec)videoCodec))
                        {
                            captureSettings.videoCodec = (VideoCodec)videoCodec;
                        }

                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
            
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Resolution");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) -
                                    ImGui::GetStyle().WindowPadding.x);
            if (ImGui::BeginCombo("##captureMenuResolutionCombo", captureSettings.resolution.ToString().c_str()))
            {
                for (auto resolution : captureManager.GetSupportedResolutions())
                {
                    bool isSelected = captureSettings.resolution == resolution;
                    if (ImGui::Selectable(resolution.ToString().c_str(),
                                          captureSettings.resolution == resolution))
                    {
                        captureSettings.resolution = resolution;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Framerate");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) -
                                    ImGui::GetStyle().WindowPadding.x);
            if (ImGui::BeginCombo("##captureMenuFramerateCombo", std::format("{0}", captureSettings.framerate).c_str()))
            {
                for (auto framerate : captureManager.GetSupportedFramerates())
                {
                    bool isSelected = captureSettings.framerate == framerate;
                    if (ImGui::Selectable(std::format("{0}", framerate).c_str(),
                                          captureSettings.framerate == framerate))
                    {
                        captureSettings.framerate = framerate;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::EndDisabled();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y * 5);
            auto label = captureManager.IsCapturing() ? ICON_FA_STOP " Stop" : ICON_FA_CIRCLE " Capture";
            if (ImGui::Button(label, ImVec2(ImGui::GetFontSize() * 6, ImGui::GetFontSize() * 2)))
            {
                if (captureManager.IsCapturing())
                    captureManager.StopCapture();
                else
                    captureManager.StartCapture();
            }

            ImGui::SameLine();

            ImGui::BeginDisabled(captureManager.IsCapturing());

            // TODO: Unicode directories will break this
            if (ImGui::Button(ICON_FA_FOLDER_OPEN " Browse",
                                ImVec2(ImGui::GetFontSize() * 6, ImGui::GetFontSize() * 2)))
            {
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
                                std::filesystem::path outputPath(pszFilePath);
                                LOG_INFO("Set recording output directory to: {}", outputPath.string());
                                PreferencesConfiguration::Get().captureOutputDirectory = outputPath;
                                Configuration::Get().Write(true);
                            }

                            CoTaskMemFree(pszFilePath);
                            pItem->Release();
                        }
                    }
                    pFileOpen->Release();
                } 
            }

            ImGui::EndDisabled();

            if (!captureManager.IsFFmpegPresent())
            {
                ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y * 4));
                ImGui::PushStyleColor(ImGuiCol_Text, {249.0f / 255.0f, 22.0f / 255.0f, 22.0f / 255.0f, 1.0f});
                ImGui::PushFont(UIManager::Get().GetBoldFont());
                ImGui::TextWrapped("Could not find ffmpeg. Please restart the mod through the codmvm launcher!");
                ImGui::PopFont();
                ImGui::PopStyleColor();
            }

            ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y * 4));
            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::Text("Output Directory");
            ImGui::PopFont();
            const auto& outputDirectory = PreferencesConfiguration::Get().captureOutputDirectory;
            ImGui::TextWrapped(outputDirectory.string().c_str());

            if (captureManager.IsCapturing())
            {
                ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y * 4));
                ImGui::PushFont(UIManager::Get().GetBoldFont());
                ImGui::Text("Progress");
                ImGui::PopFont();
                ImGui::Text("Captured %d frames", captureManager.GetCapturedFrameCount());
                auto totalFrames = (captureSettings.endTick - captureSettings.startTick) * (captureSettings.framerate/1000.0f);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetColorU32(ImGuiCol_Button));
                ImGui::ProgressBar((float)captureManager.GetCapturedFrameCount() / totalFrames, ImVec2(-1, 0), "");
                ImGui::PopStyleColor();
            }

            ImGui::End();
        }
    }

    void CaptureMenu::Release()
    {
    }
}  // namespace IWXMVM::UI