#include "StdInclude.hpp"
#include "CaptureManager.hpp"

#include "Mod.hpp"
#include "Utilities/PathUtils.hpp"
#include "D3D9.hpp"
#include "Events.hpp"

namespace IWXMVM::Components
{
    std::atomic<std::int32_t> captureLock;
    constexpr int32_t SHOULD_CAPTURE_FRAME = -1;
    constexpr int32_t FINISHED_CAPTURE = 1;

    std::string_view CaptureManager::GetOutputFormatLabel(OutputFormat outputFormat)
    {
        switch (outputFormat)
        {
            case OutputFormat::Video:
                return "Video";
            case OutputFormat::CameraData:
                return "Camera Data";
            case OutputFormat::ImageSequence:
                return "Image Sequence";
            default:
                return "Unknown Output Format";
        }
    }

    std::string_view CaptureManager::GetVideoCodecLabel(VideoCodec codec)
    {
        switch (codec)
        {
            case VideoCodec::Uncompressed:
                return "Uncompressed";
            case VideoCodec::Prores4444XQ:
                return "Prores 4444 XQ";
            case VideoCodec::Prores4444:
                return "Prores 4444";
            case VideoCodec::Prores422HQ:
                return "Prores 422 HQ";
            case VideoCodec::Prores422:
                return "Prores 422";
            case VideoCodec::Prores422LT:
                return "Prores 422 LT";
            default:
                return "Unknown Video Codec";
        }
    }

    void CaptureManager::Initialize()
    {
        captureSettings = {
            Camera::Mode::FirstPerson,
            -1,
            -1,
            OutputFormat::Video, 
            VideoCodec::Prores4444,
            Resolution(1920, 1080),
            250
        };

        outputDirectory = std::filesystem::path(PathUtils::GetCurrentGameDirectory()) / "IWXMVM" / "recordings";

        Events::RegisterListener(EventType::OnDemoLoad, [&]() {
            if (captureSettings.startTick == -1 || captureSettings.endTick == -1)
            {
                auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
                captureSettings.startTick = endTick * 0.1;
                captureSettings.endTick = endTick * 0.9;
            }
        });

        Events::RegisterListener(EventType::OnFrame, [&]() { OnRenderFrame(); });
    }

    void CaptureManager::OnRenderFrame()
    {
        if (!isCapturing)
            return;

        if (captureLock.load() == SHOULD_CAPTURE_FRAME)
        {
            if (!D3D9::CaptureBackBuffer(captureTexture))
            {
                LOG_ERROR("Failed to capture back buffer");
                StopCapture();
                return;
            }

            switch (captureSettings.outputFormat)
            {
                case OutputFormat::ImageSequence:
                {
                    std::string filename =
                        std::format("{0}/{1}_{2}.tga", imageSequenceDirectory.string(), "frame", imageSequenceFrame++);
                    auto hresult = D3DXSaveTextureToFileA(filename.c_str(), D3DXIFF_TGA, captureTexture, NULL);
                    if (hresult != D3D_OK)
                    {
                        LOG_ERROR("Failed to save texture to file {}", hresult);
                        StopCapture();
                        return;
                    }
                    break;
                }
                default:
                    LOG_ERROR("Output format not supported yet");
                    break;
            }
            

            auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
            if (currentTick > captureSettings.endTick)
            {
                StopCapture();
            }

            captureLock.store(FINISHED_CAPTURE);
        }
    }

    int32_t CaptureManager::OnGameFrame()
    {
        // wait until frame is captured
        if (captureLock.load() == SHOULD_CAPTURE_FRAME)
            return 0;

        captureLock.store(SHOULD_CAPTURE_FRAME);
        return 1000 / GetCaptureSettings().framerate;
    }

    void CaptureManager::ToggleCapture()
    {
        if (!isCapturing)
        {
            StartCapture();
        }
        else
        {
            StopCapture();
        }
    }

    void CaptureManager::StartCapture()
    {
        if (captureSettings.startTick >= captureSettings.endTick)
        {
            LOG_ERROR("Start tick must be less than end tick");
            return;
        }

        // ensure output directory exists
        if (!std::filesystem::exists(outputDirectory))
        {
            std::filesystem::create_directories(outputDirectory);
        }

        // TODO: skip to start tick

        Components::CameraManager::Get().SetActiveCamera(captureSettings.cameraMode);

        if (!D3D9::CreateTexture(captureTexture,
                                 ImVec2(captureSettings.resolution.width, captureSettings.resolution.height)))
        {
            LOG_ERROR("Failed to create capture texture");
            return;
        }


        LOG_INFO("Starting capture");

        switch (captureSettings.outputFormat)
        {
            case OutputFormat::ImageSequence:
                imageSequenceDirectory = outputDirectory / "image_sequence";
                if (!std::filesystem::exists(imageSequenceDirectory))
                {
                    std::filesystem::create_directories(imageSequenceDirectory);
                }
                imageSequenceFrame = 0;
                break;
            default:
                LOG_ERROR("Output format not supported yet");
                break;
        }

        isCapturing = true;
    }

    void CaptureManager::StopCapture()
    {
        LOG_INFO("Stopping capture");
        isCapturing = false;

        if (captureLock.load() == SHOULD_CAPTURE_FRAME)
        {
            captureLock.store(FINISHED_CAPTURE);
        }

        if (captureTexture)
        {
            captureTexture->Release();
            captureTexture = nullptr;
        }

        switch (captureSettings.outputFormat)
        {
            case OutputFormat::ImageSequence:
                break;
            default:
                LOG_ERROR("Output format not supported yet");
                break;
        }
    }
}