#pragma once

namespace IWXMVM::IW3::Hooks::Playback
{
    struct dataSizes
    {
        uint32_t centities = 72 * 476;
        uint32_t compass = 64 * 48;
        uint32_t chat = 1320;
        uint32_t commands = 128 * 1024;
        uint32_t gamestate = 140844;
    };
    static constexpr dataSizes g_dataSizes;

    struct customSnapshot
    {
        bool populated = false;
        int fileOffset = 0;

        int serverTime = 0;
        int serverConfigDataSequence = 0;
        int lastExecutedServerCommand = 0;
        int serverCommandSequence1 = 0;
        int serverCommandSequence2 = 0;

        char gameState[g_dataSizes.gamestate]{};
    };

    enum filestreamState
    {
        Uninitialized,
        InitializationFailed,
        Initialized
    };

    void SkipForward(std::int32_t ticks);
    void RewindBy(std::int32_t ticks);
    void Install();

    void Reset();

    extern std::atomic<std::int32_t> rewindTo;
}