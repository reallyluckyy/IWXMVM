#pragma once

namespace IWXMVM::Components
{
    namespace Rewinding
    {
        bool CheckSkipForward();
        bool IsRewinding();
        void RewindBy(std::int32_t ticks);

        int FS_Read(void* buffer, int len);

        void Initialize();
    } // namespace Rewinding
}  // namespace IWXMVM::Components