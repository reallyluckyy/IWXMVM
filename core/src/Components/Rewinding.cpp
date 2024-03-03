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
    std::atomic<std::int32_t> rewindTo = -1;

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
        rewindTo.store(-1);
    }

    void RestoreOldGamestate()
    {
        static std::int32_t latestRewindTo = 0;

        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();

        if (latestRewindTo > 0)
        {
            auto curTime = *reinterpret_cast<int*>(addresses.cl.snap_serverTime);
            if (curTime + 1000 >= latestRewindTo && curTime >= initialGamestate->serverTime + 1000)
            {
                if (curTime < latestRewindTo)
                {
                    Playback::SkipForward(latestRewindTo - curTime);
                    *reinterpret_cast<int*>(addresses.cl.snap_serverTime) += latestRewindTo - curTime;
                }

                LOG_DEBUG("Finished rewinding. Requested server time: {}, actual server time: {}", latestRewindTo,
                          *reinterpret_cast<int*>(addresses.cl.snap_serverTime));
                latestRewindTo = 0;
                rewindTo = -1;
            }

            return;
        }

        if (initialGamestate != nullptr)
        {
            latestRewindTo = rewindTo.exchange(-2);
            assert(latestRewindTo != -1);

            if (latestRewindTo - initialGamestate->serverTime <= 0)
            {
                // not sure why this happens yet, but this is an invalid value!
                LOG_DEBUG("Cannot rewind past first tick: {}, instead rewinding to: {}",
                          latestRewindTo - initialGamestate->serverTime, initialGamestate->serverTime);
                latestRewindTo = initialGamestate->serverTime;
            }

            Mod::GetGameInterface()->ResetClientData(latestRewindTo);
            Mod::GetGameInterface()->CL_FirstSnapshot();

            LOG_DEBUG("Rewound and time is now: {}", latestRewindTo - initialGamestate->serverTime);
            demoFileOffset = initialGamestate->fileOffset;
            demoFile.seekg(demoFileOffset);

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
        }
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
                demoFileSize = demoFile.tellg();
                demoFile.seekg(0, std::ios::beg);

                filestreamState = FilestreamState::Initialized;
                LOG_DEBUG("Opened file stream for demo file: {}", demoPath);
            }
        }

        if (filestreamState != FilestreamState::Initialized)
            return -1;

        // only reset when the game has just requested the one byte message type
        auto wouldReadDemoFooter = demoFileOffset + 9 >= demoFileSize;
        if (len == 1 && (rewindTo != -1 || wouldReadDemoFooter))
        {
            if (wouldReadDemoFooter)
            {
                auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();

                LOG_DEBUG("Reached the footer / end of the demo, rewinding and pausing now");
                rewindTo.store(*reinterpret_cast<int*>(addresses.cl.snap_serverTime) - 1000);

                if (!Components::Playback::IsPaused())
                    Components::Playback::TogglePaused();
            }
            RestoreOldGamestate();
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

    bool IsRewinding()
    {
        return rewindTo.load() != -1;
    }

    void RewindBy(std::int32_t ticks)
    {
        auto addresses = Mod::GetGameInterface()->GetPlaybackDataAddresses();

        // only set the rewind value if the main thread isn't using it
        auto curRewindTo = rewindTo.load();
        if (rewindTo == -2 ||
            !rewindTo.compare_exchange_strong(curRewindTo, *reinterpret_cast<int*>(addresses.cl.serverTime) + ticks))
        {
            LOG_DEBUG("Attempted to rewind back {} ticks", ticks);
            return;
        }

        LOG_DEBUG("Rewinding back {} ticks", ticks);
    }

    void Initialize()
    {
        Events::RegisterListener(EventType::PreDemoLoad, ResetRewindData);
    }
}  // namespace IWXMVM::Components::Playback
