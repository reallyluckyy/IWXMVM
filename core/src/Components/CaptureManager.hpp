#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
    enum OutputFormat
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

    struct CaptureSettings
    {
        Camera::Mode cameraMode;
        int32_t startTick, endTick;
        OutputFormat outputFormat;
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
        void StartCapture();

        std::string_view GetOutputFormatLabel(OutputFormat outputFormat);
        
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

       private:
        CaptureManager()
        {
        }

        CaptureSettings captureSettings;
    };
}  // namespace IWXMVM::Components