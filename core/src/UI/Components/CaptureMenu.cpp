#include "StdInclude.hpp"
#include "CaptureMenu.hpp"

#include "Resources.hpp"
#include "Graphics/Graphics.hpp"
#include "Events.hpp"
#include "Mod.hpp"
#include "UI/UIManager.hpp"
#include "Components/CaptureManager.hpp"
#include "Components/CameraManager.hpp"
#include "Utilities/PathUtils.hpp"
#include "Configuration/PreferencesConfiguration.hpp"

namespace IWXMVM::UI
{
    enum class MenuMode
    {
        SinglePass,
        MultiPass
    };

    constexpr auto fieldLayoutPercentage = 0.4f;
    std::optional<int32_t> displayPassIndex = std::nullopt;

    std::optional<int32_t> CaptureMenu::GetDisplayPassIndex() const
    {
        return displayPassIndex;
    }

    void CaptureMenu::Initialize()
    {
    }

    void DrawStreamsSection(Components::CaptureSettings& captureSettings)
    {
        using namespace Components;

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Indent();

        for (auto it = captureSettings.passes.begin(); it != captureSettings.passes.end();)
        {
            auto i = std::distance(captureSettings.passes.begin(), it);
            ImGui::PushID(std::format("##pass{}", i).c_str());

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetFontSize() * 0.1f);
            ImGui::AlignTextToFramePadding();
            ImGui::Text(ICON_FA_LAYER_GROUP "  Pass %d", i);

            auto comboWidth = ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) -
                              ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().ItemSpacing.x * 2 -
                              ImGui::GetFontSize() * 1.2f * 2;
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(comboWidth);
            if (ImGui::BeginCombo("##passTypeCombo", magic_enum::enum_name(it->type).data()))
            {
                for (auto p = 0; p < (int)PassType::Count; p++)
                {
                    bool isSelected = it->type == (PassType)p;
                    if (ImGui::Selectable(magic_enum::enum_name((PassType)p).data(), isSelected))
                    {
                        it->type = (PassType)p;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (displayPassIndex.has_value() && displayPassIndex.value() == i)
            {
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_EYE_SLASH))
                {
                    displayPassIndex = std::nullopt;
                    Components::Rendering::SetRenderingFlags(Types::RenderingFlags_DrawEverything);
                }
                else
                {
                    Components::Rendering::SetRenderingFlags(it->renderingFlags);
                }
            }
            else if (!displayPassIndex.has_value())
            {
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_EYE))
                {
                    displayPassIndex = i;
                    Components::Rendering::SetRenderingFlags(it->renderingFlags);
                }

                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_MINUS))
                {
                    captureSettings.passes.erase(it);
                    ImGui::PopID();
                    break;
                }
            }

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(comboWidth);
            if (D3D9::IsReshadePresent())
            {
                ImGui::Checkbox("Enable Reshade", &it->useReshade);
            }

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(comboWidth);
            if (ImGui::BeginTable("##selectableTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
            {
                auto DrawSelectable = [&](const char* label, Types::RenderingFlags flags) {
                    if (ImGui::Selectable(label, it->renderingFlags & flags))
                    {
                        it->renderingFlags = static_cast<Types::RenderingFlags>(it->renderingFlags ^ flags);
                    }
                };

                ImGui::TableNextColumn();
                DrawSelectable("Players", Types::RenderingFlags_DrawPlayers);
                ImGui::TableNextColumn();
                DrawSelectable("World", Types::RenderingFlags_DrawWorld);
                ImGui::TableNextColumn();
                DrawSelectable("Viewmodel", Types::RenderingFlags_DrawViewmodel);
                ImGui::TableNextColumn();
                DrawSelectable("Muzzle Flash", Types::RenderingFlags_DrawMuzzleFlash);
                ImGui::EndTable();
            }

            ImGui::Dummy(ImVec2(0, 3));

            ImGui::PopID();
            it++;
        }

        ImGui::Dummy(ImVec2(0, 3));
        if (ImGui::Button(ICON_FA_PLUS " Add Pass"))
        {
            captureSettings.passes.push_back({PassType::Default, Types::RenderingFlags_DrawEverything});
        }

        ImGui::Unindent();
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

            ImGui::BeginDisabled(captureManager.IsCapturing());

            ImGui::PushFont(UIManager::Get().GetBoldFont());
            ImGui::Text("Capture Settings");
            ImGui::PopFont();

            auto halfWidth = ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) - ImGui::GetStyle().WindowPadding.x;
            halfWidth /= 2;
            halfWidth -= ImGui::GetStyle().ItemSpacing.x / 2;
            auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Timeframe");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(halfWidth);
            ImGui::DragInt("##startTickInput", (int32_t*)&captureSettings.startTick, 10, 0, captureSettings.endTick);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(halfWidth);
            ImGui::DragInt("##endTickInput", (int32_t*)&captureSettings.endTick, 10, captureSettings.startTick, endTick);

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
            if (ImGui::BeginCombo("##captureMenuFramerateCombo", std::to_string(captureSettings.framerate).c_str()))
            {
                for (auto framerate : captureManager.GetSupportedFramerates())
                {
                    bool isSelected = captureSettings.framerate == framerate;
                    if (ImGui::Selectable(std::to_string(framerate).c_str(),
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

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Mode");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() * fieldLayoutPercentage);
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * (1 - fieldLayoutPercentage) -
                                    ImGui::GetStyle().WindowPadding.x);

            ImGui::BeginDisabled(displayPassIndex.has_value());
            static MenuMode menuMode;
            if (ImGui::BeginCombo("##captureMenuModeCombo", magic_enum::enum_name(menuMode).data()))
            {
                if (ImGui::Selectable(magic_enum::enum_name(MenuMode::SinglePass).data(),
                                      menuMode == MenuMode::SinglePass))
                {
                    menuMode = MenuMode::SinglePass;
                }

                if (ImGui::Selectable(magic_enum::enum_name(MenuMode::MultiPass).data(),
                                      menuMode == MenuMode::MultiPass))
                {
                    menuMode = MenuMode::MultiPass;
                }

                ImGui::EndCombo();
            }
            ImGui::EndDisabled();

            if (menuMode == MenuMode::MultiPass)
            {
                DrawStreamsSection(captureSettings);
            }
            else
            {
                captureSettings.passes.clear();
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

                auto totalFrames = (captureSettings.endTick - captureSettings.startTick) * (captureSettings.framerate / 1000.0f);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetColorU32(ImGuiCol_Button));

                if (CaptureManager::Get().MultiPassEnabled())
                {
                    ImGui::ProgressBar((float)(captureManager.GetCapturedFrameCount() / captureSettings.passes.size()) / totalFrames, ImVec2(-1, 0), "");
                }
                else
                {
                    ImGui::ProgressBar((float)captureManager.GetCapturedFrameCount() / totalFrames, ImVec2(-1, 0), "");
                }
                ImGui::PopStyleColor();
            }

            ImGui::End();
        }
    }

    void CaptureMenu::Release()
    {
    }
}  // namespace IWXMVM::UI