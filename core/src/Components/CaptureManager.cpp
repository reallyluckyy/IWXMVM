#include "StdInclude.hpp"
#include "CaptureManager.hpp"

#include "Mod.hpp"
#include "Components/Rewinding.hpp"
#include "Components/Playback.hpp"
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
        IDirect3DDevice9* device = D3D9::GetDevice();

        if (FAILED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
        {
            LOG_ERROR("Failed to capture backbuffer. Capture resolution not found.");
            return;
        }

        D3DSURFACE_DESC bbDesc = {};
        if (FAILED(backBuffer->GetDesc(&bbDesc)))
        {
            LOG_ERROR("Failed to get backbuffer description. Capture resolution not found.");
            return;
        }

        backBuffer->Release();
        backBuffer = nullptr;

        const Resolution gameResolution = {
            static_cast<std::int32_t>(bbDesc.Width),
            static_cast<std::int32_t>(bbDesc.Height),
        };
        for (std::ptrdiff_t i = 0; i < std::ssize(supportedResolutions); i++)
        {
            supportedResolutions[i] = {
                gameResolution.width / (i + 1),
                gameResolution.height / (i + 1),
            };
        }

        captureSettings = {
            0,
            0,
            OutputFormat::Video, 
            VideoCodec::Prores4444,
            gameResolution,
            250
        };

        outputDirectory = std::filesystem::path(PathUtils::GetCurrentGameDirectory()) / "IWXMVM" / "recordings";

        Events::RegisterListener(EventType::OnDemoBoundsDetermined, [&]() {
            if (captureSettings.startTick == 0 || captureSettings.endTick == 0)
            {
                auto endTick = Mod::GetGameInterface()->GetDemoInfo().endTick;
                captureSettings.startTick = static_cast<int32_t>(endTick * 0.1);
                captureSettings.endTick = static_cast<int32_t>(endTick * 0.9);
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
            IDirect3DDevice9* device = D3D9::GetDevice();
            if (FAILED(device->StretchRect(backBuffer, NULL, downsampledRenderTarget, NULL, D3DTEXF_NONE)))
            {
                LOG_ERROR("Failed to copy data from backbuffer to render target");
                StopCapture();
                return;
            }

            if (FAILED(device->GetRenderTargetData(downsampledRenderTarget, tempSurface)))
            {
                LOG_ERROR("Failed copy render target data to surface");
                StopCapture();
                return;
            }

            D3DLOCKED_RECT lockedRect = {};
            if (FAILED(tempSurface->LockRect(&lockedRect, nullptr, 0)))
            {
                LOG_ERROR("Failed to lock surface");
                StopCapture();
                return;
            }

            const auto surfaceByteSize = screenDimensions.width * screenDimensions.height * 4;
            std::fwrite(lockedRect.pBits, surfaceByteSize, 1, pipe);

            capturedFrameCount++;

            if (FAILED(tempSurface->UnlockRect()))
            {
                LOG_ERROR("Failed to unlock surface");
                StopCapture();
                return;
            }

            const auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
            if (!Rewinding::IsRewinding() && currentTick > captureSettings.endTick)
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

    std::filesystem::path GetFFmpegPath()
    {
        auto appdataPath = std::filesystem::path(getenv("APPDATA"));
        return appdataPath / "codmvm_launcher" / "ffmpeg.exe";
    }

    std::string GetFFmpegCommand(const Components::CaptureSettings& captureSettings, const std::filesystem::path& outputDirectory, const Resolution screenDimensions)
    {
        auto path = GetFFmpegPath();
        switch (captureSettings.outputFormat)
        {
            case OutputFormat::ImageSequence:
                return std::format(
                    "{} -f rawvideo -pix_fmt bgra -s {}x{} -r {} -i - -q:v 0 "
                    "-vf scale={}:{} -y \"{}\\output_%06d.tga\" > ffmpeg_log.txt 2>&1",
                    path.string(),
                    screenDimensions.width, screenDimensions.height, captureSettings.framerate,
                    captureSettings.resolution.width, captureSettings.resolution.height, outputDirectory.string());
            case OutputFormat::Video:
            {
                std::int32_t profile = 0;
                const char* pixelFormat = nullptr;
                switch (captureSettings.videoCodec.value())
                {
                    case VideoCodec::Prores4444XQ:
                        profile = 5;
                        pixelFormat = "yuv444p10le";
                        break;
                    case VideoCodec::Prores4444:
                        profile = 4;
                        pixelFormat = "yuv444p10le";
                        break;
                    case VideoCodec::Prores422HQ:
                        profile = 3;
                        pixelFormat = "yuv422p10le";
                        break;
                    case VideoCodec::Prores422:
                        profile = 2;
                        pixelFormat = "yuv422p10le";
                        break;
                    case VideoCodec::Prores422LT:
                        profile = 1;
                        pixelFormat = "yuv422p10le";
                        break;
                    default:
                        profile = 4;
                        pixelFormat = "yuv444p10le";
                        LOG_ERROR("Unsupported video codec. Choosing default ({})",
                                  static_cast<std::int32_t>(VideoCodec::Prores4444));
                        break;
                }

                std::string filename = "output.mov";
                auto i = 0;
                while (std::filesystem::exists(outputDirectory / filename))
                {
                    filename = std::format("output{0}.mov", ++i);
                }

                return std::format(
                    "{} -f rawvideo -pix_fmt bgra -s {}x{} -r {} -i - -c:v prores -profile:v {} -q:v 1 "
                    "-pix_fmt {} -vf scale={}:{} -y \"{}\\{}\" > ffmpeg_log.txt 2>&1",
                    path.string(), screenDimensions.width, screenDimensions.height, captureSettings.framerate, profile,
                    pixelFormat, captureSettings.resolution.width, captureSettings.resolution.height, outputDirectory.string(), filename);
            }
            default:
                LOG_ERROR("Output format not supported");
                return "";
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

        // skip to start tick
        auto currentTick = Mod::GetGameInterface()->GetDemoInfo().currentTick;
        Playback::SetTickDelta(captureSettings.startTick - currentTick, true);

        capturedFrameCount = 0;

        LOG_INFO("Starting capture at {0} ({1} fps)", captureSettings.resolution.ToString(), captureSettings.framerate);

        IDirect3DDevice9* device = D3D9::GetDevice();

        if (FAILED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
        {
            LOG_ERROR("Failed to capture backbuffer");
            StopCapture();
            return;
        }

        D3DSURFACE_DESC bbDesc = {};
        if (FAILED(backBuffer->GetDesc(&bbDesc)))
        {
            LOG_ERROR("Failed to get backbuffer description");
            StopCapture();
            return;
        }
        if (FAILED(device->CreateOffscreenPlainSurface(bbDesc.Width, bbDesc.Height, bbDesc.Format, D3DPOOL_SYSTEMMEM,
                                                       &tempSurface, nullptr)))
        {
            LOG_ERROR("Failed to create temporary surface");
            StopCapture();
            return;
        }

        if (FAILED(device->CreateRenderTarget(bbDesc.Width, bbDesc.Height, bbDesc.Format, D3DMULTISAMPLE_NONE, 0, FALSE,
            &downsampledRenderTarget, NULL)))
        {
            LOG_ERROR("Failed to create render target");
            StopCapture();
            return;
        }

        screenDimensions.width = static_cast<std::int32_t>(bbDesc.Width);
        screenDimensions.height = static_cast<std::int32_t>(bbDesc.Height);

        std::string ffmpegCommand = GetFFmpegCommand(captureSettings, outputDirectory, screenDimensions);
        if (!std::filesystem::exists(GetFFmpegPath()))
        {
            LOG_ERROR("ffmpeg is not present in the game directory");
            ffmpegNotFound = true;
            StopCapture();
            return;
        }
        ffmpegNotFound = false;

        LOG_DEBUG("ffmpeg command: {}", ffmpegCommand);
        pipe = _popen(ffmpegCommand.c_str(), "wb");
        if (!pipe)
        {
            LOG_ERROR("ffmpeg pipe open error");
            StopCapture();
            return;
        }

        isCapturing.store(true);
    }

    void CaptureManager::StopCapture()
    {
        LOG_INFO("Stopped capture (wrote {0} frames)", capturedFrameCount);
        isCapturing.store(false);

        if (captureLock.load() == SHOULD_CAPTURE_FRAME)
        {
            captureLock.store(FINISHED_CAPTURE);
        }

        if (pipe)
        {
            fflush(pipe);
            fclose(pipe);
            pipe = nullptr;
        }

        if (tempSurface)
        {
            tempSurface->Release();
            tempSurface = nullptr;
        }

        if (backBuffer)
        {
            backBuffer->Release();
            backBuffer = nullptr;
        }

        if (downsampledRenderTarget)
        {
            downsampledRenderTarget->Release();
            downsampledRenderTarget = nullptr;
        }
    }
}