#include "StdInclude.hpp"
#include "CaptureManager.hpp"

namespace IWXMVM::Components
{
    std::string_view CaptureManager::GetOutputFormatLabel(OutputFormat outputFormat)
    {
        switch (outputFormat)
        {
            case OutputFormat::Video:
                return "Video (.avi)";
            case OutputFormat::CameraData:
                return "Camera Data (.csv)";
            default:
                return "Unknown Output Format";
        }
    }

    void CaptureManager::Initialize()
    {
        captureSettings = {
            Camera::Mode::FirstPerson,
            0,
            10,
            OutputFormat::Video, 
            Resolution(1920, 1080),
            250
        };
    }

    void CaptureManager::StartCapture()
    {
        if (captureSettings.startTick >= captureSettings.endTick)
        {
            LOG_ERROR("Start tick must be less than end tick");
            return;
        }

        LOG_DEBUG("Starting capture");
        LOG_WARN("Capture is not implemented yet");
    }
}