#pragma once

namespace IWXMVM::Components
{
    namespace KeyframeSerializer
    {
        void Write(std::filesystem::path path);
        void Read(std::filesystem::path path);
    }  // namespace KeyframeSerializer
}  // namespace IWXMVM::Components