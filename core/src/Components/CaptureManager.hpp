#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
    enum class OutputFormat
    {
        Video,
        CameraData,
        ImageSequence,

        Count
    };

    struct Resolution
    {
        int32_t width, height;

        bool operator==(const Resolution& other) const
        {
            return width == other.width && height == other.height;
        }

        std::string ToString() const
        {
            return std::format("{0}x{1}", width, height);
        }
    };

    enum class VideoCodec
    {
        Uncompressed,
        Prores4444XQ,
        Prores4444,
        Prores422HQ,
        Prores422,
        Prores422LT,

        Count
    };

    struct CaptureSettings
    {
        Camera::Mode cameraMode;
        int32_t startTick, endTick;
        
        OutputFormat outputFormat;
        std::optional<VideoCodec> videoCodec;

        Resolution resolution;
        int32_t framerate;
    };

    class CaptureManager
    {
       public:
        static CaptureManager& Get()
        {
            static CaptureManager instance;
            return instance;
        }

        CaptureManager(CaptureManager const&) = delete;
        void operator=(CaptureManager const&) = delete;

        void Initialize();
        void ToggleCapture();
        void StartCapture();
        void StopCapture();

        std::string_view GetOutputFormatLabel(OutputFormat outputFormat);
        std::string_view GetVideoCodecLabel(VideoCodec codec);
        
        CaptureSettings& GetCaptureSettings()
        {
            return captureSettings;
        }

        std::array<Components::Camera::Mode, 3> GetRecordableCameras() 
        {
            return 
            {
                Components::Camera::Mode::FirstPerson, 
                Components::Camera::Mode::Dolly,
                Components::Camera::Mode::Bone
            };
        }

        std::array<Resolution, 3> GetSupportedResolutions()
        {
            return 
            {
                Resolution{ 2560, 1440 },
                Resolution{ 1920, 1080 },
                Resolution{ 1280, 720 }
            };
        }

        std::array<int32_t, 3> GetSupportedFramerates()
        {
            return { 250, 500, 1000 };
        }

        bool IsCapturing() const
        {
            return isCapturing;
        }

        int32_t OnGameFrame();

       private:
        CaptureManager()
        {
        }

        void OnRenderFrame();

        CaptureSettings captureSettings;
        std::filesystem::path outputDirectory;
        std::atomic_bool isCapturing = false;

        // internal capture state
        IDirect3DTexture9* captureTexture = nullptr;
        std::filesystem::path imageSequenceDirectory;
        std::int32_t imageSequenceFrame = 0;
    };
}  // namespace IWXMVM::Components