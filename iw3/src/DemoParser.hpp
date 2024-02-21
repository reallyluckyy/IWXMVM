#pragma once

namespace IWXMVM::IW3::DemoParser
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

    std::tuple<int32_t, int32_t> GetDemoTickRange();
}  // namespace IWXMVM::IW3::DemoParser