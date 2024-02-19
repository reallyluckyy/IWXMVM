#include "StdInclude.hpp"
#include "Playback.hpp"

#include "Components/Playback.hpp"
#include "Utilities/HookManager.hpp"
#include "Events.hpp"
#include "../Addresses.hpp"
#include "../Structures.hpp"
#include "../Functions.hpp"
#include "../Patches.hpp"

namespace IWXMVM::IW3::Hooks::Playback
{
    void SV_Frame_Internal(std::int32_t& msec)
    {
        msec = Components::Playback::CalculatePlaybackDelta(msec);
    }

    void __declspec(naked) SV_Frame_Hook()
    {
        static std::int32_t msec;
        static std::int32_t ebxValue;

        __asm
        {
            pop ecx
            pushad
            mov msec, esi
            mov ebxValue, ebx
        }

        if (ebxValue == *reinterpret_cast<std::int32_t*>(0x69136C))  // not true for CoD4X
        {
            SV_Frame_Internal(msec);
            __asm
            {
                popad
                mov eax, msec
                ret
            }
        }
        else
        {
            SV_Frame_Internal(msec);
            __asm
            {
                popad
                mov ebx, msec
                ret
            }
        }
    }

    filestreamState g_fileStreamMode = Uninitialized;
    std::ifstream g_file;
    uint32_t g_countBuffer = 0;
    std::unique_ptr<customSnapshot> g_frameData;
    std::atomic<std::int32_t> rewindTo = -1;

    struct clientActive_t  // cl
    {
        uintptr_t snap_serverTime = 0xC5F940;
        uintptr_t serverTime = 0xC628D0;
        uintptr_t oldServerTime = 0xC628D4;
        uintptr_t oldFrameServerTime = 0xC628D8;
        uintptr_t serverTimeDelta = 0xC628DC;
        uintptr_t oldSnapServerTime = 0xC628E0;
        uintptr_t parseEntitiesNum = 0xC84F58;
        uintptr_t parseClientsNum = 0xC84F5C;
    };
    clientActive_t cl;

    struct clientConnection_t  // clc
    {
        uintptr_t serverCommandSequence = 0x914E20;
        uintptr_t lastExecutedServerCommand = 0x914E24;
        //uintptr_t serverCommands = 0x914E28;
        uintptr_t timeDemoFrames = 0x934E88;
        uintptr_t timeDemoStart = 0x934E8C;
        uintptr_t timeDemoPrev = 0x934E90;
        uintptr_t timeDemoBaseTime = 0x934E94;
        uintptr_t serverConfigDataSequence = 0x9562AC;  // CoD4X
    };
    clientConnection_t clc;

    struct clientStatic_t  // cls
    {
        uintptr_t realtime = 0x956E98;
        uintptr_t realFrametime = 0x956E9C;
    };
    clientStatic_t cls;

    struct cgs_t  // cgs
    {
        uintptr_t serverCommandSequence = 0x74A91C;
        uintptr_t processedSnapshotNum = 0x74A920;
    };
    cgs_t cgs;

    struct cg_t  // cg
    {
        uintptr_t clientNum = 0x74E338;
        uintptr_t latestSnapshotNum = 0x74E350;
        uintptr_t latestSnapshotTime = 0x74E354;
        uintptr_t snap = 0x74E358;
        uintptr_t nextSnap = 0x74E35C;
        uintptr_t landTime = 0x7975F8;
    };
    cg_t cg;

    void CL_FirstSnapshotWrapper()
    {
        int clientNum = *reinterpret_cast<int*>(cg.clientNum);
        uint32_t CL_FirstSnapshot =
            *(reinterpret_cast<uint32_t*>(0x45C49A + 1)) + 0x45C49A + 5;

        Patches::GetGamePatches().Con_TimeJumped.Apply();

        _asm
        {
            pushad
            mov eax, clientNum
            call CL_FirstSnapshot
            popad
        }

        Patches::GetGamePatches().Con_TimeJumped.Revert();
    }

    void ResetOldClientData(int serverTime)
    {
        *reinterpret_cast<int*>(cl.snap_serverTime) = serverTime;
        *reinterpret_cast<int*>(cl.serverTime) = 0;
        *reinterpret_cast<int*>(cl.oldServerTime) = 0;
        *reinterpret_cast<int*>(cl.oldFrameServerTime) = 0;
        *reinterpret_cast<int*>(cl.serverTimeDelta) = 0;
        *reinterpret_cast<int*>(cl.oldSnapServerTime) = 0;

        *reinterpret_cast<int*>(clc.timeDemoFrames) = 0;
        *reinterpret_cast<int*>(clc.timeDemoStart) = 0;
        *reinterpret_cast<int*>(clc.timeDemoPrev) = 0;
        *reinterpret_cast<int*>(clc.timeDemoBaseTime) = 0;

        *reinterpret_cast<int*>(cls.realtime) = 0;
        *reinterpret_cast<int*>(cls.realFrametime) = 0;

        *reinterpret_cast<int*>(cgs.processedSnapshotNum) = 0;

        *reinterpret_cast<int*>(cg.latestSnapshotNum) = 0;
        *reinterpret_cast<int*>(cg.latestSnapshotTime) = 0;
        *reinterpret_cast<int*>(cg.snap) = 0;
        *reinterpret_cast<int*>(cg.nextSnap) = 0;
        *reinterpret_cast<int*>(cg.landTime) = 0;
    }

    void RestoreOldGamestate()
    {
        static std::int32_t latestRewindTo = 0;

        if (latestRewindTo > 0)
        {
            if (*reinterpret_cast<int*>(cl.snap_serverTime) + 1000 >= latestRewindTo &&
                *reinterpret_cast<int*>(cl.snap_serverTime) >= g_frameData->serverTime + 1000)
            {
                LOG_DEBUG("Finished rewinding. Requested server time: {}, actual server time: {}",
                          latestRewindTo, *reinterpret_cast<int*>(cl.snap_serverTime));
                latestRewindTo = 0;
                rewindTo = -1;
            }

            return;
        }

        if (g_frameData != nullptr)
        {
            latestRewindTo = rewindTo.exchange(-2);
            assert(latestRewindTo != -1);

            if (latestRewindTo - g_frameData->serverTime <= 0)
            {
                // not sure why this happens yet, but this is an invalid value!
                LOG_DEBUG("Cannot rewind past first tick: {}", latestRewindTo - g_frameData->serverTime);
                rewindTo = -1;
                return;
            }

            ResetOldClientData(latestRewindTo);
            CL_FirstSnapshotWrapper();

            LOG_DEBUG("Rewinded and time is now: {}", latestRewindTo - g_frameData->serverTime);
            g_countBuffer = g_frameData->fileOffset;
            g_file.seekg(g_countBuffer);

            *reinterpret_cast<int*>(cl.parseEntitiesNum) = 0;
            *reinterpret_cast<int*>(cl.parseClientsNum) = 0;
            *reinterpret_cast<int*>(clc.serverConfigDataSequence) = g_frameData->serverConfigDataSequence;
            *reinterpret_cast<int*>(clc.lastExecutedServerCommand) = g_frameData->lastExecutedServerCommand;
            *reinterpret_cast<int*>(clc.serverCommandSequence) = g_frameData->serverCommandSequence1;
            *reinterpret_cast<int*>(cgs.serverCommandSequence) = g_frameData->serverCommandSequence2;
            *reinterpret_cast<int*>(0x8EEB50) = 0;
            memset(reinterpret_cast<char*>(0x84F2D8), 0, g_dataSizes.centities);
            //memset(reinterpret_cast<char*>(0x74B798), 0, g_dataSizes.chat);
            //memset(reinterpret_cast<char*>(clc.serverCommands), 0, g_dataSizes.commands);
            memset(reinterpret_cast<char*>(0x742B20), 0, g_dataSizes.compass);
            memcpy(reinterpret_cast<char*>(0xC628EC), &g_frameData->gameState, g_dataSizes.gamestate);
        }
    }

    bool SetupFileStream(char* fileName)
    {
        std::string filePath = fileName;

        if (filePath.length() < 2)
            return false;

        g_file.open(Mod::GetGameInterface()->GetDemoInfo().path, std::ios::binary);
        return g_file.is_open();
    }

    void StoreCurrentGamestate(int len)
    {
        // TODO: find a more robust method of detecting a gamestate message
        if (len >= 10'000)
        {
            // first message with the gamestate; triggers the game to load a map
            if (g_frameData != nullptr)
            {
                // clear old data in case this not the first gamestate in the demo
                g_frameData.reset();
            }
        } 
        else if (g_frameData == nullptr)
        {
            // after gamestate before first snapshot
            g_frameData = std::make_unique<customSnapshot>();
            g_frameData->fileOffset = g_countBuffer - 9;

            g_frameData->serverConfigDataSequence = *reinterpret_cast<int*>(clc.serverConfigDataSequence);
            g_frameData->lastExecutedServerCommand = *reinterpret_cast<int*>(clc.lastExecutedServerCommand);
            g_frameData->serverCommandSequence1 = *reinterpret_cast<int*>(clc.serverCommandSequence);
            g_frameData->serverCommandSequence2 = *reinterpret_cast<int*>(cgs.serverCommandSequence);

            memcpy(&g_frameData->gameState, reinterpret_cast<char*>(0xC628EC), g_dataSizes.gamestate);
        }
        else if (!g_frameData->populated)
        {
            // after first snapshot and before second snapshot
            g_frameData->populated = true;
            g_frameData->serverTime = *reinterpret_cast<int*>(cl.snap_serverTime);
        }
    }

    typedef int (*FS_Read_t)(void* buffer, int len, int f);
    FS_Read_t FS_Read_Trampoline;
    int FS_Read_Hook(void* buffer, int len, int f)
    {
        using namespace Structures;
        fileHandleData_t fh =
            *reinterpret_cast<fileHandleData_t*>(GetGameAddresses().fsh() + f * sizeof(fileHandleData_t));
       
        //
        // TODO handle file closing and resetting data properly
        if (g_frameData != nullptr && g_frameData->serverTime != 0 &&
            Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
        {
            const std::string_view sv =
                reinterpret_cast<fileHandleData_t*>(GetGameAddresses().fsh() + 1 * sizeof(fileHandleData_t))->name;

            if (!sv.ends_with(Mod::GetGameInterface()->GetDemoExtension()))
            {
                LOG_DEBUG("Closing file handle and resetting rewind data.");
                g_fileStreamMode = Uninitialized;
                g_file.close();
                g_countBuffer = 0;
                g_frameData.reset();
                rewindTo.store(-1);
            }
        }
        //

        std::string_view sv = fh.name;
        bool isDemoFile = sv.ends_with(Mod::GetGameInterface()->GetDemoExtension());

        if (!isDemoFile)
        {
            return FS_Read_Trampoline(buffer, len, f);
        }
        
        if (g_fileStreamMode == Uninitialized)
        {
            if (!SetupFileStream(fh.name))
            {
                g_fileStreamMode = InitializationFailed;
                LOG_ERROR("Failed to open file stream for demo file: {}", fh.name);
            }
            else
            {
                g_fileStreamMode = Initialized;
                LOG_DEBUG("Opened file stream for demo file: {}", fh.name);
            }
        }

        if (g_fileStreamMode != Initialized)
            return FS_Read_Trampoline(buffer, len, f);

        if (len == 1 && rewindTo != -1)  // only reset when the game has just requested the one byte message type
            RestoreOldGamestate();
        else if (len > 12)
            // to exclude client archives (and CoD4X protocol header)
            StoreCurrentGamestate(len);

        g_file.read(reinterpret_cast<char*>(buffer), len);
        g_countBuffer += len;

        assert(g_countBuffer == g_file.tellg());  // gets triggered when a demo is loaded when playing another demo!
        return len;
    }

    void SkipForward(std::int32_t ticks)
    {
        Structures::GetClientStatic()->realtime += ticks;
        LOG_DEBUG("Skipping forward {} ticks, realtime: {}", ticks, Structures::GetClientStatic()->realtime);
    }

    void RewindBy(std::int32_t ticks)
    {
        // only set the rewind value if the main thread isn't using it
        auto curRewindTo = rewindTo.load();
        if (rewindTo == -2 ||
            !rewindTo.compare_exchange_strong(curRewindTo, *reinterpret_cast<int*>(cl.serverTime) + ticks))
        {
            LOG_DEBUG("Attempted to rewind back {} ticks", ticks);
            return;
        }

        // If playback is paused, skip forward a little bit to trigger
        // a demo file read, which will restore the gamestate.
        // This is admittedly a bit of a hack.
        if (Components::Playback::IsPaused())
        {
            LOG_DEBUG("Demo was paused while rewinding, so skipping 200 ms forward.");
            SkipForward(200);
        }

        LOG_DEBUG("Rewinding back {} ticks", ticks);
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().SV_Frame(), (std::uintptr_t)SV_Frame_Hook, nullptr);

        HookManager::CreateHook(GetGameAddresses().FS_Read(), (std::uintptr_t)FS_Read_Hook, (uintptr_t*)&FS_Read_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::Playback
