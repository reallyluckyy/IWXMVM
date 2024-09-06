#pragma once

namespace IWXMVM::IW5::Structures
{
    union DvarValue
    {
        const char* string;
        std::uint16_t unsignedShort;
        std::int16_t word;
        std::uint32_t unsignedInteger;
        std::int32_t integer;
        float value;
        byte boolean;
        float vec2[2];
        float vec3[3];
        float vec4[4];
        std::uint8_t color[4];
    };

    struct DvarLimits
    {
        std::int32_t min;
        std::int32_t max;
    };

    struct dvar_t
    {
        const char* name;
        int32_t flags;
        int8_t type;
        bool modified;
        std::uint8_t _unk1[2];
        DvarValue current;
        DvarValue latched;
        DvarValue defaultValue;
        DvarLimits domain;
        dvar_t* next;
        dvar_t* hashNext;
    };

    enum netadrtype_t
    {
        NA_BOT = 0x0,
        NA_BAD = 0x1,
        NA_LOOPBACK = 0x2,
        NA_BROADCAST = 0x3,
        NA_IP = 0x4,
        NA_IPX = 0x5,
        NA_BROADCAST_IPX = 0x6,
    };

    struct netadr_t
    {
        netadrtype_t type;
        uint8_t ip[4];
        uint16_t port;
        uint8_t sa_netnum[4];
        uint8_t sa_nodenum[6];
        uint32_t routerHandle;
    };

    struct clientDemoMain_t
    {
        char demoName[256];
        int demoEnumFileIndex;
        void* demoFileHandle;
        int state;
        // ... (incomplete)
    };

    struct clientConnection_t
    {
        int qport;
        int clientNum;
        int lastPacketSentTime;
        int lastPacketTime;
        netadr_t serverAddress;
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
        clientDemoMain_t clientDemo;
        uint8_t unk1[0x3FC4];
        // ...
    };

    struct clientDemoKeyFrame_t
    {
        int keyframeBufferStart;
        int demoFileOffset;
        int keyframeSnapshotTime;
        int keyframeClipServerTime;
        int keyframeSnapshotMessageNum;
        int keyframeSnapshotDeltaNum;
        int keyframeFirstCmdSequence;
        int size;
        int lastClientArchiveIndex;
        int currentClientArchiveIndex;
        int originalFilmTime;
        int clipServerTimeAtSegmentStart;
    };

    struct clientDemoTaggedPlayers_t
    {
        int count;
        int playerIndex[30];
    };

    struct clientDemoMetaInfo_t
    {
        char gameType[256];
        char mapName[256];
        char name[22];
        char description[22];
        bool isModifiedName;
        bool isModifiedDescription;
        int createTime;
        clientDemoTaggedPlayers_t taggedPlayers;
    };

    struct score_t
    {
        int client;
        int score;
        int ping;
        int deaths;
        int team;
        int kills;
        int rank_mp;
        int assists;
        int skill;
        void* hPartyIcon;
        void* hStatusIcon;
        void* hRankIcon;
    };

    struct clientDemoScoreInformation_t
    {
        int numScores;
        int teamScores[4];
        int teamPlayers[4];
        score_t scores[18];
    };

    struct clipSegment_t
    {
        char name[22];
        int clipBufferIndex;
        int size;
        int transition;
        int duration;
        int thumbnailImageSize;
        unsigned __int8 thumbnailImageBuf[65536];
        clientDemoTaggedPlayers_t taggedPlayers;
    };

    struct __declspec(align(4)) clientDemoPlayback_t
    {
        int32_t localClientNum;
        unsigned int maxMemoryUsed;
        bool completed;
        bool shutdownInProgress;
        bool overridePause;
        bool firstFrameEventsProcessed;
        bool jumpTimeFlag;
        int lastGeneratedKeyframeTime;
        int lastInvalidatedKeyframe;
        int totalBytesRead;
        float timeScale; // ---------------------- below this might be incorrect ----------------------
        bool hideDemoHud;
        int buttonPressTime[10];
        unsigned __int8 persistentDataBuffer[12284];
        unsigned __int8 keyframeBuf[2097152];
        clientDemoKeyFrame_t keyFrame[250];
        int keyframeIndex;
        int keyframeBufferIndex;
        int keyframeBufferReadIndex;
        int screenshotSize;
        unsigned __int8 screenshotBuf[1081344];
        int thumbnailImageSize;
        unsigned __int8 thumbnailImageBuf[65536];
        clientDemoMetaInfo_t screenshotInfo;
        int clipState;
        bool clipModified;
        int transitionScreenTime;
        unsigned __int8 clipRecordBuf[2097152];
        int clipRecordBufIndex;
        clientDemoMetaInfo_t clipRecordInfo;
        unsigned __int8 clipTransitionBuffer[16];
        int clipGamestateSize;
        int clipTime;
        int clipServerTime;
        int prevSnapServerTime;
        int originalFilmTime;
        int clipServerTimeAtSegmentStart;
        clientDemoScoreInformation_t scoresInfo;
        bool pendingScoresCopy;
        bool readMultipleNonDeltaSnapshots;
        int prevClipRecordBufIndex;
        int prevClipTime;
        bool forceWriteClipCommands;
        bool writeConfigStringClipCommand;
        int resetSnapshotClipCommandMessageNum;
        int resetSnapshotClipCommandServerCmdSeq;
        int resetSnapshotClipCommandServerTime;
        unsigned __int8 clipRecordClient;
        float clipRecordTimescale;
        unsigned __int8 clipCameraMode;
        float clipCameraOrigin[3];
        float clipCameraAngles[3];
        bool clipPausedState;
        float prevClipCameraOrigin[3];
        float prevClipCameraAngles[3];
        bool forceTeleportClipCamera;
        clipSegment_t segments[10];
        bool pendingSegmentSave;
        int segmentCount;
        int clipPreviewSegmentNumber;
        int clipPreviewBufferIndex;
        bool cancelPreview;
        bool previewAllSegments;
        int selectedRestorePoint;
        int restorePointBufferIndex;
        unsigned __int8 restorePointBuf[2][98304];
        clientDemoKeyFrame_t restoreKeyframe[2];
        bool completeStateForRestorePoint[2];
        int8_t padding[0x2046]; // padding i had to add to make things line up
        int cameraMode; // ---------------------- should be correct again from here ----------------------
        bool cameraModeChanged;
    };

    // I believe this is originally just part of cg_s
    struct demoCameraData_t
    {
        float demoCameraOrigin[3];
        float demoCameraAngles[3];
        float demoCameraVelocity[3];
    };

    struct GfxViewport
    {
        int x;
        int y;
        int width;
        int height;
    };

    struct RefdefView
    {
        float tanHalfFovX;
        float tanHalfFovY;
        float org[3];
        float axis[3][3];
        float zNear;
    };

    struct refdef_t
    {
        GfxViewport displayViewport;
        RefdefView view;
        float viewOffset[3];
        int time;
        float blurRadius;
        bool uiBlur;
        // ...
    };

    

    union Weapon
    {
        unsigned int data;
    };

    struct PlayerVehicleState
    {
        int entity;
        int flags;
        float origin[3];
        float angles[3];
        float velocity[3];
        float angVelocity[3];
        float tilt[2];
        float tiltVelocity[2];
    };

    struct playerEvents_t
    {
        int eventSequence;
        int events[4];
        unsigned int eventParms[4];
        int oldEventSequence;
        int timeADSCameUp;
    };

    enum ViewLockTypes : __int32
    {
        PLAYERVIEWLOCK_NONE = 0x0,
        PLAYERVIEWLOCK_FULL = 0x1,
        PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
        PLAYERVIEWLOCKCOUNT = 0x3,
    };

    struct SprintState
    {
        int sprintButtonUpRequired;
        int sprintDelay;
        int lastSprintStart;
        int lastSprintEnd;
        int sprintStartMaxLength;
    };

    struct MantleState
    {
        float yaw;
        int timer;
        int transIndex;
        int flags;
    };

    struct PlayerActiveWeaponState
    {
        int weapAnim;
        int weaponTime;
        int weaponDelay;
        int weaponRestrictKickTime;
        int weaponState;
        int weapHandFlags;
        unsigned int weaponShotCount;
    };

    struct PlayerEquippedWeaponState
    {
        bool usedBefore;
        bool dualWielding;
        bool inAltMode;
        bool needsRechamber[2];
        int zoomLevelIndex;
    };

    enum OffhandClass : __int32
    {
        OFFHAND_CLASS_NONE = 0x0,
        OFFHAND_CLASS_FRAG_GRENADE = 0x1,
        OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
        OFFHAND_CLASS_FLASH_GRENADE = 0x3,
        OFFHAND_CLASS_THROWINGKNIFE = 0x4,
        OFFHAND_CLASS_OTHER = 0x5,
        OFFHAND_CLASS_COUNT = 0x6,
    };

    enum PlayerHandIndex : __int32
    {
        WEAPON_HAND_RIGHT = 0x0,
        WEAPON_HAND_LEFT = 0x1,
        NUM_WEAPON_HANDS = 0x2,
        WEAPON_HAND_DEFAULT = 0x0,
    };

    struct GlobalAmmo
    {
        int ammoType;
        int ammoCount;
    };

    struct ClipAmmo
    {
        int clipIndex;
        int ammoCount[2];
    };

    struct clientInfo_t
    {
        byte pad[0x159];
    };

    struct snapshot_s
    {
        struct PlayerState
        {
            int commandTime;
            int pm_type;
        } ps;

        byte pad[0x33F3C - 0x8];
    };

    struct cg_s
    {
        byte padding[0x335C];
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
        int groundEntityTime;
        int mapRestart;
        int renderingThirdPerson;
        float landChange;
        int landTime;
        float heightToCeiling;
        refdef_t refdef;
        float refdefViewAngles[3];
        float baseGunAngles[3];
        float aimAssistEyeOrigin[3];
        float aimAssistViewOrigin[3];
        float aimAssistViewAngles[3];
        float thirdPersonGunPitch;
        float thirdPersonGunYaw;
        float thirdPersonAdsLerp;
        float swayViewAngles[3];
        float swayAngles[3];
        float swayOffset[3];
        float recoilAngles[3];
        float recoilSpeed[3];
        float demoCameraOrigin[3];
        float demoCameraAngles[3];
        float demoCameraVelocity[3];
        byte padding2[0x6884 + 0x4AC8];
        int scoreFadeTime;
        int scoresTop;
        // ...
    };

    // so snap is at 0x904304
    // and cg is at 0x900FA0

    struct clientStatic_t
    {
        int quit;
        int hunkUsersStarted;
        char servername[256];
        int rendererStarted;
        int soundStarted;
        int inUpdateSound;
        int uiStarted;
        int devGuiStarted;
        int frametime;
        float frametime_base;
        int realtime;
        byte padding[0x412658 - 0x128];  // this is quite the big buffer
        int skelTimeStamp;
        // ...
    };

    struct PlayerWeaponCommonState
    {
        Weapon offHand;
        OffhandClass offhandPrimary;
        OffhandClass offhandSecondary;
        Weapon weapon;
        int weapFlags;
        float fWeaponPosFrac;
        float aimSpreadScale;
        int adsDelayTime;
        int spreadOverride;
        int spreadOverrideState;
        float fAimSpreadMovementScale;
        PlayerHandIndex lastWeaponHand;
        GlobalAmmo ammoNotInClip[15];
        ClipAmmo ammoInClip[15];
        int weapLockFlags;
        int weapLockedEntnum;
        float weapLockedPos[3];
        int weaponIdleTime;
        Weapon lastStowedWeapon;
    };

    enum ActionSlotType : __int32
    {
        ACTIONSLOTTYPE_DONOTHING = 0x0,
        ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
        ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
        ACTIONSLOTTYPE_NIGHTVISION = 0x3,
        ACTIONSLOTTYPECOUNT = 0x4,
    };

    struct ActionSlotParam_SpecifyWeapon
    {
        Weapon weapon;
    };

    struct ActionSlotParam
    {
        ActionSlotParam_SpecifyWeapon specifyWeapon;
    };

    enum objectiveState_t : __int32
    {
        OBJST_EMPTY = 0x0,
        OBJST_ACTIVE = 0x1,
        OBJST_INVISIBLE = 0x2,
        OBJST_DONE = 0x3,
        OBJST_CURRENT = 0x4,
        OBJST_FAILED = 0x5,
        OBJST_NUMSTATES = 0x6,
    };

    struct objective_t
    {
        objectiveState_t state;
        float origin[3];
        int entNum;
        int teamNum;
        int clientNum;
        int invertVisibilityByClientNum;
        int icon;
    };

    struct hudelem_s
    {
        __int32 type;
        float x;
        float y;
        float z;
        int targetEntNum;
        float fontScale;
        float fromFontScale;
        int fontScaleStartTime;
        int fontScaleTime;
        int font;
        int alignOrg;
        int alignScreen;
        int color;
        int fromColor;
        int fadeStartTime;
        int fadeTime;
        int label;
        int width;
        int height;
        int materialIndex;
        int fromWidth;
        int fromHeight;
        int scaleStartTime;
        int scaleTime;
        float fromX;
        float fromY;
        int fromAlignOrg;
        int fromAlignScreen;
        int moveStartTime;
        int moveTime;
        int time;
        int duration;
        float value;
        int text;
        float sort;
        int glowColor;
        int fxBirthTime;
        int fxLetterTime;
        int fxDecayStartTime;
        int fxDecayDuration;
        int soundID;
        int flags;
    };

    /* 9517 */
    struct playerState_s_hud
    {
        hudelem_s current[31];
        hudelem_s archival[31];
    };

    struct playerState_s
    {
        int commandTime;
        int pm_type;
        int pm_time;
        int pm_flags;
        int otherFlags;
        int linkFlags;
        int bobCycle;
        float origin[3];
        float velocity[3];
        int grenadeTimeLeft;
        int throwbackGrenadeOwner;
        int throwbackGrenadeTimeLeft;
        Weapon throwbackWeapon;
        int remoteEyesEnt;
        int remoteEyesTagname;
        int remoteControlEnt;
        int remoteTurretEnt;
        int foliageSoundTime;
        int gravity;
        int speed;
        float delta_angles[3];
        int groundEntityNum;
        float vLadderVec[3];
        int jumpTime;
        float jumpOriginZ;
        int legsTimer;
        int legsAnim;
        int torsoTimer;
        int torsoAnim;
        int legsAnimDuration;
        int torsoAnimDuration;
        int damageTimer;
        int damageDuration;
        int flinchYawAnim;
        int corpseIndex;
        PlayerVehicleState vehicleState;
        int movementDir;
        int eFlags;
        playerEvents_t pe;
        int unpredictableEventSequence;
        int unpredictableEventSequenceOld;
        int unpredictableEvents[4];
        unsigned int unpredictableEventParms[4];
        int clientNum;
        int viewmodelIndex;
        float viewangles[3];
        int viewHeightTarget;
        float viewHeightCurrent;
        int viewHeightLerpTime;
        int viewHeightLerpTarget;
        int viewHeightLerpDown;
        float viewAngleClampBase[2];
        float viewAngleClampRange[2];
        int damageEvent;
        int damageYaw;
        int damagePitch;
        int damageCount;
        int damageFlags;
        int stats[4];
        float proneDirection;
        float proneDirectionPitch;
        float proneTorsoPitch;
        ViewLockTypes viewlocked;
        int viewlocked_entNum;
        float linkAngles[3];
        float linkWeaponAngles[3];
        int linkWeaponEnt;
        int loopSound;
        int cursorHint;
        int cursorHintString;
        int cursorHintEntIndex;
        int cursorHintDualWield;
        int iCompassPlayerInfo;
        int radarEnabled;
        int radarBlocked;
        int radarMode;
        int radarStrength;
        int radarShowEnemyDirection;
        int locationSelectionInfo;
        SprintState sprintState;
        float holdBreathScale;
        int holdBreathTimer;
        float moveSpeedScaleMultiplier;
        MantleState mantleState;
        PlayerActiveWeaponState weapState[2];
        Weapon weaponsEquipped[15];
        PlayerEquippedWeaponState weapEquippedData[15];
        PlayerWeaponCommonState weapCommon;
        int meleeChargeDist;
        int meleeChargeTime;
        int meleeChargeEnt;
        unsigned int airburstMarkDistance;
        unsigned int perks[2];
        unsigned int perkSlots[9];
        ActionSlotType actionSlotType[4];
        ActionSlotParam actionSlotParam[4];
        int weaponHudIconOverrides[6];
        int animScriptedType;
        int shellshockIndex;
        int shellshockTime;
        int shellshockDuration;
        float viewKickScale;
        float dofNearStart;
        float dofNearEnd;
        float dofFarStart;
        float dofFarEnd;
        float dofNearBlur;
        float dofFarBlur;
        float dofViewmodelStart;
        float dofViewmodelEnd;
        objective_t objective[32];
        int deltaTime;
        int killCamEntity;
        int killCamLookAtEntity;
        int killCamClientNum;
        playerState_s_hud hud;
        unsigned int partBits[6];
        int recoilScale;
        int diveDirection;
        int stunTime;
        int8_t padding[0xC];
    };

    struct clSnapshot_t
    {
        playerState_s ps;
        byte pad[0x14];
        int valid;
        byte pad2[0x4];
        int snapFlags;
        int serverTime;
        int messageNum; // 0x1065AB0
        int deltaNum;
        int ping;
        int cmdNum;
        int numEntities;
        int numClients;
        int parseEntitiesIndex;
        int parseClientsIndex;
        int serverCommandNum;
    };

    // theres probably something still wrong here or in one of the sub structs
    // size should be 0x2A4918
    struct clientActive_t
    {
        bool usingAds;
        int timeoutcount;
        clSnapshot_t snap;
        bool alwaysFalse;
        byte pad[0x10]; // other stuff around here might be wrong, but the pad fixes the offset of serverTime
        int serverTime;
        int oldFrameServerTime;
        int serverTimeDelta;
        int newSnapshots;
        int serverId;
        int serverTimeErrorAvg;
        int serverTimeErrorPrev;
        int serverTimeError[20];
        int serverTimeError2[20];
        int mapcrc;
        int parseEntitiesIndex;
        int parseClientsIndex;
        int mouseDx[2];
        int mouseDy[2];
        int mouseIndex;
        byte pad2[0x9FA8];
        clSnapshot_t snapshots[32];
        // ...
    };

    struct cgs_t
    {
        int viewX;
        int viewY;
        int viewWidth;
        int viewHeight;
        float viewAspect;
        float viewMult;
        int serverCommandSequence;
        int processedSnapshotNum;
        // ...
    };

    enum entityType_t : __int32
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
        ET_TURRET = 0xB,
        ET_HELICOPTER = 0xC,
        ET_PLANE = 0xD,
        ET_VEHICLE = 0xE,
        ET_VEHICLE_COLLMAP = 0xF,
        ET_VEHICLE_CORPSE = 0x10,
        ET_VEHICLE_SPAWNER = 0x11,
        ET_EVENTS = 0x12,
    };

    struct cpose_t
    {
        uint16_t lightingHandle;
        uint8_t eType;  
        uint8_t cullIn; 
        uint32_t pad[4];        
        float origin[3];        
        float angles[3];        
        // ...
    };

    struct centity_s
    {
        cpose_t pose;
        char padding[0x134];
        struct
        {
            int32_t clientNum;  // part of entityState_s usually, i believe
        } nextState;
        char padding2[0x6C];
        uint32_t flags;
        char padding4[0x24];
    };

    struct gameState_t
    {
        byte content[0x23CAC];
    };

    struct CompassActor
    {
        int lastUpdate;
        float lastPos[3];
        float lastEnemyPos[3];
        float lastEnemyYaw;
        float lastVel;
        float lastYaw;
        int pingTime;
        int beginFadeTime;
        int beginRadarFadeTime;
        int beginVoiceFadeTime;
        int flags;
        unsigned int perks[2];
        int nextPortableRadarPingTimeList[18];
    };

    struct DSkelPartBits
    {
        int anim[6];
        int control[6];
        int worldCtrl[6];
        int skel[6];
    };

    struct DSkel
    {
        DSkelPartBits partBits;
        int timeStamp;
        void* mat; // DObjAnimMat
    };

    struct XModel
    {
        const char* name;
        unsigned __int8 numBones;
        unsigned __int8 numRootBones;
        unsigned __int8 numsurfs;
        float scale;
        unsigned int noScalePartBits[6];
        unsigned __int16* boneNames;
        unsigned __int8* parentList;
        // ...
    };

    struct DObj
    {
        void* tree;
        unsigned __int16 duplicateParts;
        unsigned __int16 entnum;
        unsigned __int8 duplicatePartsSize;
        unsigned __int8 numModels;
        unsigned __int8 numBones;
        unsigned __int8 flags;
        unsigned int ignoreCollision;
        volatile int locked;
        DSkel skel;
        float radius;
        unsigned int hidePartBits[6];
        XModel** models;
    };

    clientConnection_t* GetClientConnection();
    clientActive_t* GetClientActive();
    centity_s* GetEntities();
    cg_s* GetClientGlobals();
    cgs_t* GetClientGlobalsStatic();
    clientStatic_t* GetClientStatic();
    clientInfo_t* GetClientInfo();
    gameState_t* GetGameState();
    clientDemoPlayback_t* GetClientDemoPlayback();
}  // namespace IWXMVM::IW5::Structures
