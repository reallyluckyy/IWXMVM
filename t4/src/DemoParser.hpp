#pragma once

namespace IWXMVM::T4::DemoParser
{
    struct clientArchiveData_t
    {
        int archiveIndex;
        float origin[3];
        float velocity[3];
        int movementDir;
        int bobCycle;
        int serverTime;
        float viewAngles[3];
    };

    void Run();

    std::pair<int32_t, int32_t> GetDemoTickRange();
}  // namespace IWXMVM::T4::DemoParser