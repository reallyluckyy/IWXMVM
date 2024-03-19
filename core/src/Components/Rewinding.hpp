#pragma once

namespace IWXMVM::Components
{
    namespace Rewinding
    {
        bool IsRewinding();
        void RewindBy(std::int32_t ticks);

        bool CheckSkipForward();
        int FS_Read(void* buffer, int len);

        void Initialize();
    } // namespace Rewinding
}  // namespace IWXMVM::Components