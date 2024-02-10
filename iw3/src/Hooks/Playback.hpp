#pragma once

namespace IWXMVM::IW3::Hooks::Playback
{
    constexpr auto dummyValue = 4;

    struct dataSizes
    {
        uint32_t snapshots = 32 * 12180;
        uint32_t matchstates = dummyValue;
        uint32_t entities = 2048 * 244;
        uint32_t clients = 2048 * 100;
        uint32_t compass = 64 * 48;
        uint32_t chat = 1320;
        uint32_t gamestate = 140844;
    };
    static constexpr dataSizes g_dataSizes;

    struct customSnapshot
    {
        bool populated = false;
        int fileOffset = 0;
        int serverTime = 0;
        int landTime = 0;
        int serverConfigDataSequence = 0;

        int parseMatchStateNum = 0;
        int parseEntitiesNum = 0;
        int parseClientsNum = 0;
        char snapshots[g_dataSizes.snapshots]{};
        char matchStates[g_dataSizes.matchstates]{};
        char entities[g_dataSizes.entities]{};
        char clients[g_dataSizes.clients]{};

#ifdef enableChatRestoration
        int lastExecutedServerCommand = 0;
        int serverCommandSequence1 = 0;
        int serverCommandSequence2 = 0;
        char chat[g_dataSizes.chat]{};
#endif
#ifdef enableGamestateRestoration
        char gameState[g_dataSizes.gamestate]{};
#endif
    };

    enum filestreamState
    {
        Uninitialized,
        InitializationFailed,
        Initialized
    };


    void RequestRewindTo(std::int32_t tick);
    void Install();
}