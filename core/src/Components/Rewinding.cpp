#include "StdInclude.hpp"
#include "Rewinding.hpp"

#include "Playback.hpp"
#include "Mod.hpp"
#include "Events.hpp"

namespace IWXMVM::Components::Rewinding
{
    struct InitialGamestate
    {
        bool populated = false;
        int fileOffset = 0;

        int serverTime = 0;
        int serverConfigDataSequence = 0;
        int lastExecutedServerCommand = 0;
        int serverCommandSequence1 = 0;
        int serverCommandSequence2 = 0;

        // TODO: what if these are exceeded?
        char clientInfo[100'000]{};
        char gameState[200'000]{};
    };

    enum class FilestreamState
    {
        Uninitialized,
        InitializationFailed,
        Initialized
    };

    FilestreamState filestreamState = FilestreamState::Uninitialized;
    std::ifstream demoFile;
    uint32_t demoFileSize = 0;
    uint32_t demoFileOffset = 0;
    std::unique_ptr<InitialGamestate> initialGamestate;

    inline constexpr std::int32_t TICK_OFFSET = 150;
    inline constexpr std::int32_t NOT_IN_USE = -1;
    inline constexpr std::int32_t SKIPPING_FORWARD = -2;

    std::int32_t latestRewindTo = NOT_IN_USE;
    std::atomic<std::int32_t> rewindTo = NOT_IN_USE;

    void ResetRewindData()
    {
        LOG_DEBUG("Closing file handle and resetting rewind data");
        filestreamState = FilestreamState::Uninitialized;
        if (demoFile.is_open())
        {
            demoFile.close();
        }
        demoFileSize = 0;
        demoFileOffset = 0;
        initialGamestate.reset();
        latestRewindTo = NOT_IN_USE;
        rewindTo.store(NOT_IN_USE);
    }

    void RestoreOldGamestate(auto wouldReadDemoFooter)
    {
        if (initialGamestate == nullptr || latestRewindTo > 0)
            return;

        if (wouldReadDemoFooter)
        {
            auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();

            LOG_DEBUG("Reached the footer / end of the demo, rewinding and pausing now");
            rewindTo.store(*reinterpret_cast<int*>(addresses.cl.snap_serverTime) - 1000);

            if (!Components::Playback::IsPaused())
                Components::Playback::TogglePaused();
        } 
        else if (auto curRewindTo = rewindTo.load(); curRewindTo == NOT_IN_USE || curRewindTo == SKIPPING_FORWARD) 
        {
            // rewinding is not in progress, or the data has already been reset / restored
            // and we're waiting for the game to catch up to the requested tick
            return;
        }

        latestRewindTo = rewindTo.load();
        assert(latestRewindTo > 0);

        if (latestRewindTo - initialGamestate->serverTime <= 0)
        {
            // not sure why this happens yet, but this is an invalid value!
            LOG_DEBUG("Cannot rewind past first tick: {}, instead rewinding to: {}",
                        latestRewindTo, initialGamestate->serverTime);
            latestRewindTo = initialGamestate->serverTime;
        }

        Mod::GetGameInterface()->ResetClientData(latestRewindTo);
        Mod::GetGameInterface()->CL_FirstSnapshot();

        LOG_DEBUG("Rewound and time is now: {}", latestRewindTo - initialGamestate->serverTime);
        demoFileOffset = initialGamestate->fileOffset;
        demoFile.seekg(demoFileOffset);

        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();
        *reinterpret_cast<int*>(addresses.cl.parseEntitiesNum) = 0;
        *reinterpret_cast<int*>(addresses.cl.parseClientsNum) = 0;
        *reinterpret_cast<int*>(addresses.clc.lastExecutedServerCommand) = initialGamestate->lastExecutedServerCommand;
        *reinterpret_cast<int*>(addresses.clc.serverCommandSequence) = initialGamestate->serverCommandSequence1;
        *reinterpret_cast<int*>(addresses.cgs.serverCommandSequence) = initialGamestate->serverCommandSequence2;
        *reinterpret_cast<int*>(addresses.killfeed) = 0;

        // can be 0 as its cod4x only
        if (addresses.clc.serverConfigDataSequence)
        {
            *reinterpret_cast<int*>(addresses.clc.serverConfigDataSequence) =
                initialGamestate->serverConfigDataSequence;
        }

        memset(reinterpret_cast<char*>(addresses.s_compassActors.address), 0, addresses.s_compassActors.size);
        memset(reinterpret_cast<char*>(addresses.teamChatMsgs.address), 0, addresses.teamChatMsgs.size);
        memset(reinterpret_cast<char*>(addresses.clc.serverCommands.address), 0, addresses.clc.serverCommands.size);
        memset(reinterpret_cast<char*>(addresses.cg_entities.address), 0, addresses.cg_entities.size);
        memcpy(reinterpret_cast<char*>(addresses.clientInfo.address), initialGamestate->clientInfo,
                addresses.clientInfo.size);
        memcpy(reinterpret_cast<char*>(addresses.gameState.address), initialGamestate->gameState,
                addresses.gameState.size);

        rewindTo.store(SKIPPING_FORWARD);
    }
    
    void StoreCurrentGamestate(int len)
    {
        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();

        // TODO: find a more robust method of detecting a gamestate message
        if (len >= 10'000)
        {
            // first message with the gamestate; triggers the game to load a map
            if (initialGamestate != nullptr)
            {
                // clear old data in case this not the first gamestate in the demo
                initialGamestate.reset();
            }
        }
        else if (initialGamestate == nullptr)
        {
            // after gamestate before first snapshot
            initialGamestate = std::make_unique<InitialGamestate>();
            initialGamestate->fileOffset = demoFileOffset - 9;

            initialGamestate->lastExecutedServerCommand =
                *reinterpret_cast<int*>(addresses.clc.lastExecutedServerCommand);
            initialGamestate->serverCommandSequence1 =
                *reinterpret_cast<int*>(addresses.clc.serverCommandSequence);
            initialGamestate->serverCommandSequence2 =
                *reinterpret_cast<int*>(addresses.cgs.serverCommandSequence);
            
            // can be 0 as its cod4x only
            if (addresses.clc.serverConfigDataSequence)
            {
                initialGamestate->serverConfigDataSequence =
                    *reinterpret_cast<int*>(addresses.clc.serverConfigDataSequence);
            }

            memcpy(initialGamestate->clientInfo, reinterpret_cast<char*>(addresses.clientInfo.address),
                   addresses.clientInfo.size);
            memcpy(initialGamestate->gameState, reinterpret_cast<char*>(addresses.gameState.address), addresses.gameState.size);
        }
        else if (!initialGamestate->populated)
        {
            // after first snapshot and before second snapshot
            initialGamestate->populated = true;
            initialGamestate->serverTime = *reinterpret_cast<int*>(addresses.cl.snap_serverTime);
        }
    }

    bool CheckSkipForward()
    {
        if (rewindTo.load() != SKIPPING_FORWARD)
            return false;

        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();
        auto curTime = *reinterpret_cast<int*>(addresses.cl.serverTime);

        Components::Playback::SkipForward(latestRewindTo + TICK_OFFSET - curTime);
        latestRewindTo = NOT_IN_USE;
        rewindTo.store(NOT_IN_USE);

        return true;
    }

    bool IsRewinding()
    {
        return rewindTo.load() != NOT_IN_USE;
    }

    void RewindBy(std::int32_t ticks)
    {
        if (ticks >= SKIPPING_FORWARD)
        {
            LOG_DEBUG("Cannot rewind invalid tick value {}", ticks);
            return;
        }

        ticks -= TICK_OFFSET;
        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();

        // only set the rewind value if the main thread isn't using it
        auto curRewindTo = rewindTo.load();
        if (curRewindTo != NOT_IN_USE ||
            !rewindTo.compare_exchange_strong(curRewindTo, *reinterpret_cast<int*>(addresses.cl.serverTime) + ticks))
        {
            LOG_DEBUG("Attempted to rewind back {} ticks", ticks);
            return;
        }

        LOG_DEBUG("Rewinding back {} ticks", ticks);
    }

    int FS_Read(void* buffer, int len)
    {
        if (filestreamState == FilestreamState::Uninitialized)
        {
            auto demoPath = Mod::GetGameInterface()->GetDemoInfo().path;
            demoFile.open(demoPath, std::ios::binary);
            if (!demoFile.is_open())
            {
                filestreamState = FilestreamState::InitializationFailed;
                LOG_ERROR("Failed to open file stream for demo file: {}", demoPath);
            }
            else
            {
                demoFile.seekg(0, std::ios::end);
                demoFileSize = (uint32_t)demoFile.tellg();
                demoFile.seekg(0, std::ios::beg);

                filestreamState = FilestreamState::Initialized;
                LOG_DEBUG("Opened file stream for demo file: {}", demoPath);
            }
        }

        if (filestreamState != FilestreamState::Initialized)
            return -1;

        // only reset when the game has just requested the one byte message type
        if (len == 1)
        {
            auto wouldReadDemoFooter = demoFileOffset + 9 >= demoFileSize;
            RestoreOldGamestate(wouldReadDemoFooter);
        }
        else if (len > 12)
        {
            // to exclude client archives (and CoD4X protocol header)
            StoreCurrentGamestate(len);
        }

        demoFile.read(reinterpret_cast<char*>(buffer), len);
        demoFileOffset += len;

        // gets triggered when a demo is loaded when playing another demo!
        assert(demoFileOffset == demoFile.tellg());

        return len;
    }

    void Initialize()
    {
        Events::RegisterListener(EventType::PreDemoLoad, ResetRewindData);
    }
}  // namespace IWXMVM::Components::Playback
