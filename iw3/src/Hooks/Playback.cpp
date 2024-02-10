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
    std::stack<customSnapshot> g_frameData;
    std::ifstream g_file;
    uint32_t g_countBuffer = 0;
    uint32_t g_snapshotCount = 0;
    std::array<int, 20> g_serverTimes;
    int rewindTime = 1000;
    std::atomic<std::int32_t> rewindTo = -1;

    struct clientActive_t  // cl
    {
        uintptr_t snap_serverTime = 0xC5F940;
        uintptr_t serverTime = 0xC628D0;
        uintptr_t oldServerTime = 0xC628D4;
        uintptr_t oldFrameServerTime = 0xC628D8;
        uintptr_t serverTimeDelta = 0xC628DC;
        uintptr_t oldSnapServerTime = 0xC628E0;
        uintptr_t parseMatchStateNum = dummyValue;
        uintptr_t parseEntitiesNum = 0xC84F58;
        uintptr_t parseClientsNum = 0xC84F5C;
        uintptr_t snapshots = 0xCC9180;
        uintptr_t parseMatchStates = dummyValue;
        uintptr_t parseEntities = 0xD65400;
        uintptr_t parseClients = 0xDE5EA4;
    };
    clientActive_t cl;

    struct clientConnection_t  // clc
    {
        uintptr_t serverCommandSequence = 0x914E20;
        uintptr_t lastExecutedServerCommand = 0x914E24;
        uintptr_t demoplaying = 0x934E74;
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
    }

    void RestoreOldGamestate()
    {
        if (g_frameData.size() > 1)
        {
            g_frameData.pop();

            if (rewindTo > -1 && g_frameData.size() > 0)
            {
                while (g_frameData.size() > 1 && g_frameData.top().serverTime > rewindTo)
                {
                    LOG_DEBUG("Popping frame data with server time {}", g_frameData.top().serverTime);
                    g_frameData.pop();
                }
            }

            rewindTo = -1;
            g_serverTimes = {};
            g_countBuffer = g_frameData.top().fileOffset;
            g_file.seekg(g_countBuffer);

            // TODO: *reinterpret_cast<int*>(g_addresses.data.clearMiniConsole) = 0;
            // TODO: memset(reinterpret_cast<char*>(g_addresses.data.compassData), 0, g_dataSizes.compass);

            ResetOldClientData(g_frameData.top().serverTime);
            CL_FirstSnapshotWrapper();

            *reinterpret_cast<int*>(cg.landTime) = g_frameData.top().landTime;

            *reinterpret_cast<int*>(cl.parseEntitiesNum) = g_frameData.top().parseEntitiesNum;
            *reinterpret_cast<int*>(cl.parseClientsNum) = g_frameData.top().parseClientsNum;

            memcpy(reinterpret_cast<char*>(cl.snapshots), &g_frameData.top().snapshots,
                   sizeof(customSnapshot::snapshots));
            memcpy(reinterpret_cast<char*>(cl.parseEntities), &g_frameData.top().entities,
                   sizeof(customSnapshot::entities));
            memcpy(reinterpret_cast<char*>(cl.parseClients), &g_frameData.top().clients,
                   sizeof(customSnapshot::clients));

#if (gameID == CallofDuty4_MW)
            *reinterpret_cast<int*>(clc.serverConfigDataSequence) =
                g_frameData.top().serverConfigDataSequence;
#elif (gameID == CallofDuty7_BO1)
            *reinterpret_cast<int*>(g_addresses.data.cl.parseMatchStateNum) = g_frameData.top().parseMatchStateNum;
            memcpy(reinterpret_cast<char*>(g_addresses.data.cl.parseMatchStates), &g_frameData.top().matchStates,
                   sizeof(customSnapshot::matchStates));
#endif

#ifdef enableChatRestoration
            *reinterpret_cast<int*>(g_addresses.data.clc.lastExecutedServerCommand) =
                g_frameData.top().lastExecutedServerCommand;
            *reinterpret_cast<int*>(g_addresses.data.clc.serverCommandSequence) =
                g_frameData.top().serverCommandSequence1;
            *reinterpret_cast<int*>(g_addresses.data.cgs.serverCommandSequence) =
                g_frameData.top().serverCommandSequence2;
            memcpy(reinterpret_cast<char*>(g_addresses.data.chat), &g_frameData.top().chat,
                   sizeof(customSnapshot::chat));
#endif
#ifdef enableGamestateRestoration
            memcpy(reinterpret_cast<char*>(g_addresses.data.gameState), &g_frameData.top().gameState,
                   sizeof(customSnapshot::gameState));
#endif
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

    void StoreCurrentGamestate(int fps)
    {
        // this function is executed in a hook that precedes snapshot parsing; so the data must be stored in the most
        // recent entry on the std::stack (g_frameData)
        if ((fps && g_snapshotCount > 4 && (g_snapshotCount - 1) % (rewindTime / fps) == 0) || g_snapshotCount == 4)
        {
            if (g_frameData.size() && g_frameData.top().fileOffset && !g_frameData.top().serverTime)
            {
                g_frameData.top().populated = true;

                g_frameData.top().serverTime = *reinterpret_cast<int*>(cl.snap_serverTime);
                g_frameData.top().landTime = *reinterpret_cast<int*>(cg.landTime);

#if (gameID == CallofDuty4_MW)
                g_frameData.top().serverConfigDataSequence =
                    *reinterpret_cast<int*>(clc.serverConfigDataSequence);
#elif (gameID == CallofDuty7_BO1)
                g_frameData.top().parseMatchStateNum = *reinterpret_cast<int*>(cl.parseMatchStateNum);
                memcpy(&g_frameData.top().matchStates, reinterpret_cast<char*>(cl.parseMatchStates),
                       sizeof(customSnapshot::matchStates));
#endif

                g_frameData.top().parseEntitiesNum = *reinterpret_cast<int*>(cl.parseEntitiesNum);
                g_frameData.top().parseClientsNum = *reinterpret_cast<int*>(cl.parseClientsNum);

                memcpy(&g_frameData.top().snapshots, reinterpret_cast<char*>(cl.snapshots),
                       sizeof(customSnapshot::snapshots));
                memcpy(&g_frameData.top().entities, reinterpret_cast<char*>(cl.parseEntities),
                       sizeof(customSnapshot::entities));
                memcpy(&g_frameData.top().clients, reinterpret_cast<char*>(cl.parseClients),
                       sizeof(customSnapshot::clients));

#ifdef enableChatRestoration
                g_frameData.top().lastExecutedServerCommand =
                    *reinterpret_cast<int*>(clc.lastExecutedServerCommand);
                g_frameData.top().serverCommandSequence1 =
                    *reinterpret_cast<int*>(clc.serverCommandSequence);
                g_frameData.top().serverCommandSequence2 =
                    *reinterpret_cast<int*>(cgs.serverCommandSequence);
                memcpy(&g_frameData.top().chat, reinterpret_cast<char*>(g_addresses.data.chat),
                       sizeof(customSnapshot::chat));
#endif
#ifdef enableGamestateRestoration
                memcpy(&g_frameData.top().gameState, reinterpret_cast<char*>(g_addresses.data.gameState),
                       sizeof(customSnapshot::gameState));
#endif
            }
        }

        // don't start storing snapshots before the third one to avoid some glitch with the chat (some old chat would be
        // restored with the first snapshot)
        if ((fps && g_snapshotCount > 3 && g_snapshotCount % (rewindTime / fps) == 0) || g_snapshotCount == 3)
        {
            if (g_frameData.size() == 0)
                g_frameData.emplace();
            else if (g_frameData.top()
                         .populated)  // only create a new custom snapshot if the previous was actually used!
                g_frameData.emplace();
            else
            {
                [[maybe_unused]] int breakp = 1;  // this shouldn't happen!
            }

            g_frameData.top().fileOffset =
                g_countBuffer -
                9;  // to account for message type (byte), server message index (int) and message size (int)
        }
    }

    int DetermineFramerate(int serverTime)
    {
        // this function stores a small number of server times, and calculates the server framerate
        if (!serverTime)
            return 0;

        if (g_serverTimes[(g_snapshotCount + g_serverTimes.size() - 1) % g_serverTimes.size()] > serverTime)
        {
            g_serverTimes = {};
            g_serverTimes[g_snapshotCount % g_serverTimes.size()] = serverTime;

            return 0;
        }
        else
        {
            g_serverTimes[g_snapshotCount % g_serverTimes.size()] = serverTime;
            std::unordered_map<int, int> frequency;

            for (int i = g_snapshotCount; i < static_cast<int>(g_snapshotCount + g_serverTimes.size()); ++i)
            {
                int temp = g_serverTimes[i % g_serverTimes.size()] -
                           g_serverTimes[((i + g_serverTimes.size() - 1) % g_serverTimes.size())];

                if (temp >= 0 && temp <= 1000)
                {
                    // if (temp % 50 == 0)
                    ++frequency[temp];
                }
            }

            auto highestFrequency = std::max_element(std::begin(frequency), std::end(frequency),
                                                     [](const auto& a, const auto& b) { return a.second < b.second; });

            return highestFrequency->first;
        }
    }

    typedef int (*FS_Read_t)(void* buffer, int len, int f);
    FS_Read_t FS_Read_Trampoline;
    int FS_Read_Hook(void* buffer, int len, int f)
    {
        using namespace Structures;
        fileHandleData_t fh =
            *reinterpret_cast<fileHandleData_t*>(GetGameAddresses().fsh() + f * sizeof(fileHandleData_t));
        bool isDemoFile = false;

        std::string_view sv = fh.name;
        isDemoFile = sv.ends_with(Mod::GetGameInterface()->GetDemoExtension());

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
        {  // to exclude client archives (and CoD4X protocol header)
            ++g_snapshotCount;
            StoreCurrentGamestate(DetermineFramerate(*reinterpret_cast<int*>(0xD90BCF8))); // TODO: replace with a sig
        }

        g_file.read(reinterpret_cast<char*>(buffer), len);
        g_countBuffer += len;

        assert(g_countBuffer == g_file.tellg());  // gets triggered when a demo is loaded when playing another demo!
        return len;
    }

    void RequestRewindTo(std::int32_t tick)
    {
        LOG_DEBUG("Rewinding back {} ticks", tick);
        rewindTo = *reinterpret_cast<int*>(cl.serverTime) + tick;
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().SV_Frame(), (std::uintptr_t)SV_Frame_Hook, nullptr);

        // TODO: patch away errors
        HookManager::CreateHook(GetGameAddresses().FS_Read(), (std::uintptr_t)FS_Read_Hook, (uintptr_t*)&FS_Read_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::Playback
