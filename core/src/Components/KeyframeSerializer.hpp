#pragma once

namespace IWXMVM::Components
{
    namespace KeyframeSerializer
    {
        void Write(std::filesystem::path path);
        void Read(std::filesystem::path path, bool requireDemoMatch = false);

        void WriteRecent();
        void ReadRecent();
    }  // namespace KeyframeSerializer
}  // namespace IWXMVM::Components