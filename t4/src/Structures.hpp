#pragma once

namespace IWXMVM::T4::Structures
{

    struct cmd_function_t
    {
        cmd_function_t* next;
        char* name;
        char* autocomplete1;
        char* autocomplete2;
        void* function;
    };

    struct netadr_t
    {
        int type;
        char ip[4];
        uint16_t port;
        char ipx[10];
    };

    struct clientConnection_t
    {
        int qport;
        int clientNum;
        int lastPacketSentTime;
        int lastPacketTime;
        netadr_t serverAddress;
        int unk_1;
        int connectTime;
        int connectPacketCount;
        char serverMessage[256];
        int challenge;
        int checksumFeed;
        int reliableSequence;
        int reliableAcknowledge;
        char reliableCommands[128][1024];
        int serverMessageSequence;
        int serverCommandSequence;
        int lastExecutedServerCommand;
        char serverCommands[128][1024];
        bool isServerRestarting;
        int lastClientArchiveIndex;
        char demoName[64];
        int demorecording;
        int demoplaying;
        int isTimeDemo;
        int demowaiting;
        int firstDemoFrameSkipped;
        int demofile;
        int demoLivestreamingFile;
        int demoLivestreamingSomething;
        int timeDemoLog;
        int timeDemoFrames;
        int timeDemoStart;
        int timeDemoPrev;
        int timeDemoBaseTime;
        // ...
    };

    struct clientStatic_t
    {
        int quit;
        int hunkUsersStarted;
        char servername[256];
        int rendererStarted;
        int soundStarted;
        int uiStarted;
        int frametime;
        int realtime;
        int realFrametime;
    };

    struct playerState_s
    {
        int commandTime;
        int pm_type;
        int bobCycle;
        int pm_flags;
        int weapFlags;
        int otherFlags;
        int pad;
        int pm_time;
        float origin[3];
        float velocity[3];
        float oldVelocity[2];
        char unk[14700];
    };


    struct clSnapshot_t
    {
        int valid;
        int snapFlags;
        int serverTime;
        int pad;
        int messageNum;
        int deltaNum;
        int ping;
        int cmdNum;
        playerState_s ps;
        int numEntities;
        int numClients;
        int parseEntitiesNum;
        int parseClientsNum;
        int serverCommandNum;
    };

    struct gameState_t
    {
        int stringOffsets[3056];
        char stringData[131072];
        int dataCount;
    };

    struct usercmd_s
    {
        int serverTime;
        int buttons;
        int angles[3];
        char weapon;
        char offHandIndex;
        char forwardmove;
        char rightmove;
        float meleeChargeYaw;
        char meleeChargeDist;
        char selectedLocation[2];
        char unk[12];
    };

    struct clientActive_t
    {
        bool usingAds;
        int timeoutcount;
        clSnapshot_t snap;
        bool alwaysFalse;
        int serverTime;
        int oldServerTime;
        int oldFrameServerTime;
        int serverTimeDelta;
        int oldSnapServerTime;
        int extrapolatedSnapshot;
        int newSnapshots;
        gameState_t gameState;
        char mapname[64];
        int parseEntitiesNum;
        int parseClientsNum;
        int mouseDx[2];
        int mouseDy[2];
        int mouseIndex;
        bool stanceHeld;
        int stance;
        int stancePosition;
        int stanceTime;
        int cgameUserCmdWeapon;
        int cgameUserCmdOffHandIndex;
        float cgameFOVSensitivityScale;
        float cgameMaxPitchSpeed;
        float cgameMaxYawSpeed;
        float cgameKickAngles[3];
        float cgameOrigin[3];
        float cgameVelocity[3];
        float cgameViewangles[3];
        int cgameBobCycle;
        int cgameMovementDir;
        int cgameExtraButtons;
        int cgamePredictedDataServerTime;
        float viewangles[3];
        int serverId;
        int skelTimeStamp;
        volatile int skelMemPos;
        char skelMemory[262144];
        char *skelMemoryStart;
        bool allowedAllocSkel;
        usercmd_s cmds[128];
        int cmdNumber;
        char clientArchive[48][256];
        int clientArchiveIndex;
        char outPackets[12][32];
        clSnapshot_t snapshots[32];
    };

    struct cgs_t
    {
        int viewX;
        int viewY;
        int viewWidth;
        int viewHeight;
        float viewAspect;
        int serverCommandSequence;
        int processedSnapshotNum;
        // ...
    };

    // WARNING: This is copied over from IW3 and might not be correct!
    enum entityType_t
    {
        ET_GENERAL = 0x0,
        ET_PLAYER = 0x1,
        ET_PLAYER_CORPSE = 0x2,
        ET_ITEM = 0x3,
        ET_MISSILE = 0x4,
        ET_INVISIBLE = 0x5,
        ET_SCRIPTMOVER = 0x6,
        ET_SOUND_BLEND = 0x7,
        ET_FX = 0x8,
        ET_LOOP_FX = 0x9,
        ET_PRIMARY_LIGHT = 0xA,
        ET_MG42 = 0xB,
        ET_HELICOPTER = 0xC,
        ET_PLANE = 0xD,
        ET_VEHICLE = 0xE,
        ET_VEHICLE_COLLMAP = 0xF,
        ET_VEHICLE_CORPSE = 0x10,
        ET_EVENTS = 0x11,
    };

    struct entityState_s
    {
        int number;
        int eType;
        char unk[168];
        int clientNum;
        char unk2[52];
        int weapon;
        int weaponModel;
        int legsAnim;
        int torsoAnim;
        char unk3[36];
    };
    static_assert(sizeof(entityState_s) == 0x11C, "entityState_s size mismatch");

    struct clientState_s
    {
        char unk[148];
    };

    struct snapshot_s
    {
        int snapFlags;
        int ping;
        int serverTime;
        playerState_s ps;
        int pad;
        int numEntities;
        int numClients;
        entityState_s entities[512];
        clientState_s clients[64];
        int serverCommandSequence;
    };
    static_assert(sizeof(snapshot_s) == 0x296C8, "snapshot_s size mismatch");

    struct playerEntity_t
    {
        float fLastWeaponPosFrac;
        int bPositionToADS;
        float vPositionLastOrg[3];
        float fLastIdleFactor;
        float vLastMoveOrg[3];
        float vLastMoveAng[3];
    };
    static_assert(sizeof(playerEntity_t) == 0x30, "playerEntity_t size mismatch");

    
    struct DSkelPartBits
    {
        int anim[4];
        int control[4];
        int skel[4];
    };

    struct DSkel
    {
        DSkelPartBits partBits;
        int timeStamp;
        void* mat;
    };

    // TODO: verify if this is actually correct
    struct XModel
    {
        const char* name;
        char numBones;
        char numRootBones;
        char numsurfs;
        char lodRampType;
        unsigned __int16* boneNames;
        char* parentList;
        __int16* quats;
        float* trans;
        char* partClassification;
        // ...
    };

    // TODO: verify if this is actually correct
    struct DObj_s
    {
        void* tree;
        unsigned __int16 duplicateParts;
        unsigned __int16 entnum;
        char duplicatePartsSize;
        char numModels;
        char numBones;
        unsigned int ignoreCollision;
        volatile int locked;
        DSkel skel;
        float radius;
        unsigned int hidePartBits[4];
        XModel** models;
        int pad;
    };
    static_assert(sizeof(DObj_s) == 0x68, "DObj_s size mismatch");

    struct clientInfo_t
    {
        char pad[1372];
    };
    static_assert(sizeof(clientInfo_t) == 0x55C, "clientInfo_t size mismatch");

    struct bgs_t
    {
        int unk[138902];
        int time;
        int latestSnapshotTime;
        int frametime;
        int anim_user;
        void*(__cdecl* GetXModel)(const char*);
        void(__cdecl* CreateDObj)(void*, unsigned __int16, void*, int, int, void*);
        unsigned __int16(__cdecl* AttachWeapon)(void*, unsigned __int16, void*);
        DObj_s*(__cdecl* GetDObj)(int, int);
        void(__cdecl* SafeDObjFree)(int, int);
        void*(__cdecl* AllocXAnim)(int);
        clientInfo_t clientinfo[64];
    };

    struct refdef_s
    {
        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        float tanHalfFovX;
        float tanHalfFovY;
        char pad[8];
        float vieworg[3];
        char pad2[4];
        float viewaxis[3][3];
        float viewOffset[3];
        int time;
        float zNear;
        float blurRadius;
        char unk[16432];
        char unk2[572];
        int localClientNum;
    };

    struct cpose_t
    {
        unsigned __int16 lightingHandle;
        char eType;
        char eTypeUnion;
        char localClientNum;
        int cullIn;
        char isRagdoll;
        int ragdollHandle;
        int killcamRagdollHandle;
        int physObjId;
        int pad[3];
        float origin[3];
        float angles[3];
        char skinCacheEntry[12];
        char ___u12[36];
        char unk[152];
    };

    struct centity_s
    {
        cpose_t pose;
        char currentState[96];
        entityState_s nextState;
        bool nextValid;
        bool bMuzzleFlash;
        bool bTrailMade;
        int previousEventSequence;
        int miscTime;
        float lightingOrigin[3];
        void* tree;
        int pad[7];
    };
    static_assert(sizeof(centity_s) == 0x2BC, "centity_s size mismatch");

    struct cg_s
    {
        int clientNum;
        int localClientNum;
        int demoType;
        int cubemapShot;
        int cubemapSize;
        int renderScreen;
        int latestSnapshotNum;
        int latestSnapshotTime;
        snapshot_s* snap;
        snapshot_s* nextSnap;
        snapshot_s activeSnapshots[2];
        float frameInterpolation;
        int frametime;
        int time;
        int oldTime;
        int physicsTime;
        int mapRestart;
        int renderingThirdPerson;
        playerState_s predictedPlayerState;
        centity_s predictedPlayerEntity;
        playerEntity_t playerEntity;
        int predictedErrorTime;
        float predictedError[3];
        float landChange;
        int landTime;
        float heightToCeiling;
        refdef_s refdef;
        char unk[12];
        float refdefViewAngles[3];
        float lastVieworg[3];
        char pad[12296];
        int pad2[24];
        float zoomSensitivity;
        int pad3[1154];
        int _unk01;
        int _unk02;
        bgs_t bgs;
    };

    struct directory_t
    {
        char path[256];
        char gamedir[256];
    };

    struct searchpath_s
    {
        searchpath_s* next;
        void* iwd; // iwd_t*
        directory_t* dir;
        int bLocalized;
        int ignore;
        int ignorePureCheck;
        int language;
    };

    union DvarValue
    {
        bool enabled;
        int integer;
        unsigned int unsignedInt;
        float value;
        float vector[4];
        const char* string;
        char color[4];
    };

    /* 757 */
    union DvarLimits
    {
        struct
        {
            int stringCount;
            const char** strings;
        } enumeration;
        struct
        {
            int min;
            int max;
        } integer;
        struct
        {
            float min;
            float max;
        } value;
    };

    struct dvar_s
    {
        char* name;
        const char* description;
        unsigned __int16 flags;
        char type;
        char unk_1;
        bool modified;
        DvarValue current;
        int unk_2;
        DvarValue latched;
        int unk_3;
        DvarValue reset;
        int unk_4;
        int unk_5;
        DvarLimits domain;
        dvar_s* next;
        dvar_s* hashNext;
    };

    struct __declspec(align(4)) WinMouseVars_t
    {
        int oldButtonState;
        tagPOINT oldPos;
        bool mouseActive;
        bool mouseInitialized;
    };

    struct clientUIActive_t
    {
        bool active;
        bool isRunning;
        bool cgameInitialized;
        bool cgameInitCalled;
        int keyCatchers;
        bool displayHUDWithKeycatchUI;
        int connectionState;
    };

    union qfile_gus
    {
        void* o;
        char* z;
    };

    struct qfile_us
    {
        qfile_gus file;
        int iwdIsClone;
    };

    struct fileHandleData_t
    {
        qfile_us handleFiles;
        int handleSync;
        int fileSize;
        int zipFilePos;
        void* zipFile; // iwd_t*
        int streamed;
        char name[256];
    };

    struct GfxDrawSurf
    {
        // TODO: 
    };

    struct GfxCmdBufSourceState
    {
        // TODO:
    };

    struct GfxCmdBufState
    {
        // TODO:
    };

    struct Material
    {
        // TODO: ...
    };

    clientConnection_t* GetClientConnection();
    clientStatic_t* GetClientStatic();
    clientActive_t* GetClientActive();
    cgs_t* GetClientGlobalsStatic();
    cg_s* GetClientGlobals();
    WinMouseVars_t* GetMouseVars();
    clientUIActive_t* GetClientUIActives();
    centity_s* GetEntities();
    uint16_t* GetClientObjectMap();
    DObj_s* GetObjBuf();
}  // namespace IWXMVM::T4::Structures
