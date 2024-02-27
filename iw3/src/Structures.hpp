#pragma once

namespace IWXMVM::IW3::Structures
{

    struct cmd_function_t
    {
        cmd_function_t* next;
        char* name;
        char* autocomplete1;
        char* autocomplete2;
        void* function;
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
        char ip[4];
        unsigned __int16 port;
        char ipx[10];
    };

    enum netsrc_t
    {
        NS_CLIENT1 = 0x0,
        NS_SERVER = 0x1,
        NS_MAXCLIENTS = 0x1,
        NS_PACKET = 0x2,
    };

    struct netProfilePacket_t
    {
        int iTime;
        int iSize;
        int bFragment;
    };

    struct netProfileStream_t
    {
        netProfilePacket_t packets[60];
        int iCurrPacket;
        int iBytesPerSecond;
        int iLastBPSCalcTime;
        int iCountedPackets;
        int iCountedFragments;
        int iFragmentPercentage;
        int iLargestPacket;
        int iSmallestPacket;
    };

    struct netProfileInfo_t
    {
        netProfileStream_t send;
        netProfileStream_t recieve;
    };

    struct netchan_t
    {
        int outgoingSequence;
        netsrc_t sock;
        int dropped;
        int incomingSequence;
        netadr_t remoteAddress;
        int qport;
        int fragmentSequence;
        int fragmentLength;
        char* fragmentBuffer;
        int fragmentBufferSize;
        int unsentFragments;
        int unsentFragmentStart;
        int unsentLength;
        char* unsentBuffer;
        int unsentBufferSize;
        netProfileInfo_t prof;
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
        // ...
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
        int lastClientArchiveIndex;
        char demoName[64];
        int demorecording;
        int demoplaying;
        int isTimeDemo;
        int demowaiting;
        int firstDemoFrameSkipped;
        int demofile;
        int timeDemoLog;
        int timeDemoFrames;
        int timeDemoStart;
        int timeDemoPrev;
        int timeDemoBaseTime;
        netchan_t netchan;
        char netchanOutgoingBuffer[2048];
        char netchanIncomingBuffer[131072];
        netProfileInfo_t OOBProf;
        char statPacketsToSend;
        int statPacketSendTime[7];
    };

    enum OffhandSecondaryClass
    {
        PLAYER_OFFHAND_SECONDARY_SMOKE = 0x0,
        PLAYER_OFFHAND_SECONDARY_FLASH = 0x1,
        PLAYER_OFFHAND_SECONDARIES_TOTAL = 0x2,
    };

    enum ViewLockTypes
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

    enum ActionSlotType
    {
        ACTIONSLOTTYPE_DONOTHING = 0x0,
        ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
        ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
        ACTIONSLOTTYPE_NIGHTVISION = 0x3,
        ACTIONSLOTTYPECOUNT = 0x4,
    };

    struct ActionSlotParam_SpecifyWeapon
    {
        unsigned int index;
    };

    /* 120 */
    struct ActionSlotParam
    {
        ActionSlotParam_SpecifyWeapon specifyWeapon;
    };

    enum objectiveState_t
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
        int icon;
    };

    struct hudelem_s
    {
        std::byte content[0xA0];
    };

    struct hudelements_s
    {
        hudelem_s current[31];
        hudelem_s archival[31];
    };

    struct playerState_s
    {
        int commandTime;
        int pm_type;
        int bobCycle;
        int pm_flags;
        int weapFlags;
        int otherFlags;
        int pm_time;
        float origin[3];
        float velocity[3];
        float oldVelocity[2];
        int weaponTime;
        int weaponDelay;
        int grenadeTimeLeft;
        int throwBackGrenadeOwner;
        int throwBackGrenadeTimeLeft;
        int weaponRestrictKickTime;
        int foliageSoundTime;
        int gravity;
        float leanf;
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
        int movementDir;
        int eFlags;
        int eventSequence;
        int events[4];
        unsigned int eventParms[4];
        int oldEventSequence;
        int clientNum;
        int offHandIndex;
        OffhandSecondaryClass offhandSecondary;
        unsigned int weapon;
        int weaponstate;
        unsigned int weaponShotCount;
        float fWeaponPosFrac;
        int adsDelayTime;
        int spreadOverride;
        int spreadOverrideState;
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
        int stats[5];
        int ammo[128];
        int ammoclip[128];
        unsigned int weapons[4];
        unsigned int weaponold[4];
        unsigned int weaponrechamber[4];
        float proneDirection;
        float proneDirectionPitch;
        float proneTorsoPitch;
        ViewLockTypes viewlocked;
        int viewlocked_entNum;
        int cursorHint;
        int cursorHintString;
        int cursorHintEntIndex;
        int iCompassPlayerInfo;
        int radarEnabled;
        int locationSelectionInfo;
        SprintState sprintState;
        float fTorsoPitch;
        float fWaistPitch;
        float holdBreathScale;
        int holdBreathTimer;
        float moveSpeedScaleMultiplier;
        MantleState mantleState;
        float meleeChargeYaw;
        int meleeChargeDist;
        int meleeChargeTime;
        int perks;
        ActionSlotType actionSlotType[4];
        ActionSlotParam actionSlotParam[4];
        int entityEventSequence;
        int weapAnim;
        float aimSpreadScale;
        int shellshockIndex;
        int shellshockTime;
        int shellshockDuration;
        float dofNearStart;
        float dofNearEnd;
        float dofFarStart;
        float dofFarEnd;
        float dofNearBlur;
        float dofFarBlur;
        float dofViewmodelStart;
        float dofViewmodelEnd;
        int hudElemLastAssignedSoundID;
        objective_t objective[16];
        char weaponmodels[128];
        int deltaTime;
        int killCamEntity;
        hudelements_s hud;
    };

    struct refdef_s
    {
        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        float tanHalfFovX;
        float tanHalfFovY;
        float vieworg[3];
        float viewaxis[3][3];
        float viewOffset[3];
        int time;
        float zNear;
        float blurRadius;
        byte filler[0x4038];
    };

    struct clSnapshot_t
    {
        int valid;
        int snapFlags;
        int serverTime;
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
        int stringOffsets[2442];
        char stringData[131072];
        int dataCount;
    };

    enum StanceState
    {
        CL_STANCE_STAND = 0x0,
        CL_STANCE_CROUCH = 0x1,
        CL_STANCE_PRONE = 0x2,
    };

    struct __declspec(align(2)) usercmd_s
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
    };

    struct ClientArchiveData
    {
        int serverTime;
        float origin[3];
        float velocity[3];
        int bobCycle;
        int movementDir;
        float viewangles[3];
    };

    struct outPacket_t
    {
        int p_cmdNumber;
        int p_serverTime;
        int p_realtime;
    };

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

    /* 255 */
    enum trType_t
    {
        TR_STATIONARY = 0x0,
        TR_INTERPOLATE = 0x1,
        TR_LINEAR = 0x2,
        TR_LINEAR_STOP = 0x3,
        TR_SINE = 0x4,
        TR_GRAVITY = 0x5,
        TR_ACCELERATE = 0x6,
        TR_DECELERATE = 0x7,
        TR_PHYSICS = 0x8,
        TR_FIRST_RAGDOLL = 0x9,
        TR_RAGDOLL = 0x9,
        TR_RAGDOLL_GRAVITY = 0xA,
        TR_RAGDOLL_INTERPOLATE = 0xB,
        TR_LAST_RAGDOLL = 0xB,
    };

    /* 723 */
    struct trajectory_t
    {
        trType_t trType;
        int trTime;
        int trDuration;
        float trBase[3];
        float trDelta[3];
    };

    /* 1003 */
    struct LerpEntityStateTurret
    {
        float gunAngles[3];
    };

    /* 1001 */
    struct LerpEntityStateLoopFx
    {
        float cullDist;
        int period;
    };

    /* 1009 */
    struct LerpEntityStatePrimaryLight
    {
        char colorAndExp[4];
        float intensity;
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
    };

    /* 931 */
    struct LerpEntityStatePlayer
    {
        float leanf;
        int movementDir;
    };

    /* 926 */
    struct LerpEntityStateVehicle
    {
        float bodyPitch;
        float bodyRoll;
        float steerYaw;
        int materialTime;
        float gunPitch;
        float gunYaw;
        int teamAndOwnerIndex;
    };

    /* 1011 */
    struct LerpEntityStateMissile
    {
        int launchTime;
    };

    /* 1012 */
    struct LerpEntityStateSoundBlend
    {
        float lerp;
    };

    /* 1008 */
    struct LerpEntityStateBulletHit
    {
        float start[3];
    };

    /* 924 */
    struct LerpEntityStateEarthquake
    {
        float scale;
        float radius;
        int duration;
    };

    /* 1002 */
    struct LerpEntityStateCustomExplode
    {
        int startTime;
    };

    /* 1007 */
    struct LerpEntityStateExplosion
    {
        float innerRadius;
        float magnitude;
    };

    /* 1019 */
    struct LerpEntityStateExplosionJolt
    {
        float innerRadius;
        float impulse[3];
    };

    /* 929 */
    struct LerpEntityStatePhysicsJitter
    {
        float innerRadius;
        float minDisplacement;
        float maxDisplacement;
    };

    /* 1004 */
    struct LerpEntityStateAnonymous
    {
        int data[7];
    };

    /* 1133 */
    union LerpEntityStateTypeUnion
    {
        LerpEntityStateTurret turret;
        LerpEntityStateLoopFx loopFx;
        LerpEntityStatePrimaryLight primaryLight;
        LerpEntityStatePlayer player;
        LerpEntityStateVehicle vehicle;
        LerpEntityStateMissile missile;
        LerpEntityStateSoundBlend soundBlend;
        LerpEntityStateBulletHit bulletHit;
        LerpEntityStateEarthquake earthquake;
        LerpEntityStateCustomExplode customExplode;
        LerpEntityStateExplosion explosion;
        LerpEntityStateExplosionJolt explosionJolt;
        LerpEntityStatePhysicsJitter physicsJitter;
        LerpEntityStateAnonymous anonymous;
    };

    /* 1140 */
    struct LerpEntityState
    {
        int eFlags;
        trajectory_t pos;
        trajectory_t apos;
        LerpEntityStateTypeUnion u;
    };

    union $C889CF518587CB2833BFE41358FA5E4A
    {
        int brushmodel;
        int item;
        int xmodel;
        int primaryLight;
    };

    union $0AC61FC53F35A99FE97BBC85FAE448D4
    {
        int scale;
        int eventParm2;
        int helicopterStage;
    };

    /* 1143 */
    union $F73F8AB0498479EECE844017F4CFA302
    {
        int hintString;
        int vehicleXModel;
    };

    struct entityState_s
    {
        int number;
        entityType_t eType;
        LerpEntityState lerp;
        int time2;
        int otherEntityNum;
        int attackerEntityNum;
        int groundEntityNum;
        int loopSound;
        int surfType;
        $C889CF518587CB2833BFE41358FA5E4A index;
        int clientNum;
        int iHeadIcon;
        int iHeadIconTeam;
        int solid;
        unsigned int eventParm;
        int eventSequence;
        int events[4];
        int eventParms[4];
        int weapon;
        int weaponModel;
        int legsAnim;
        int torsoAnim;
        $0AC61FC53F35A99FE97BBC85FAE448D4 un1;
        $F73F8AB0498479EECE844017F4CFA302 un2;
        float fTorsoPitch;
        float fWaistPitch;
        unsigned int partBits[4];
    };

    enum team_t
    {
        TEAM_FREE = 0x0,
        TEAM_AXIS = 0x1,
        TEAM_ALLIES = 0x2,
        TEAM_SPECTATOR = 0x3,
        TEAM_NUM_TEAMS = 0x4,
    };

    struct clientState_s
    {
        int clientIndex;
        team_t team;
        int modelindex;
        int attachModelIndex[6];
        int attachTagIndex[6];
        char name[16];
        float maxSprintTimeMultiplier;
        int rank;
        int prestige;
        int perks;
        int attachedVehEntNum;
        int attachedVehSlotIndex;
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
        StanceState stance;
        StanceState stancePosition;
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
        char* skelMemoryStart;
        bool allowedAllocSkel;
        __declspec(align(4)) usercmd_s cmds[128];
        int cmdNumber;
        ClientArchiveData clientArchive[256];
        int clientArchiveIndex;
        outPacket_t outPackets[32];
        clSnapshot_t snapshots[32];
        entityState_s entityBaselines[1024];
        entityState_s parseEntities[2048];
        clientState_s parseClients[2048];
        int corruptedTranslationFile;
        char translationVersion[256];
        float vehicleViewYaw;
        float vehicleViewPitch;
    };

    struct XModel
    {
        const char* name;
        char numBones;
        char numRootBones;
        char numsurfs;
        char lodRampType;
        unsigned __int16* boneNames;
        // ...
    };
    
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
        void* mat;  // DObjAnimMat
    };

    struct DObj_s
    {
        void* tree;  // XAnimTree_s*
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
        XModel** models;  // XModel**
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
        int localServer;
        char gametype[32];
        char szHostName[256];
        int maxclients;
        char mapname[64];
        int gameEndTime;
        int voteTime;
        int voteYes;
        int voteNo;
        char voteString[256];
        XModel* gameModels[512];
        void* fxs[100];
        void* smokeGrenadeFx;
        byte holdBreathParams[0x268];
        char teamChatMsgs[8][160];
        int teamChatMsgTimes[8];
        int teamChatPos;
        int teamLastChatPos;
        float compassWidth;
        float compassHeight;
        float compassY;
        // ...
    };

    enum DemoType
    {
        DEMO_TYPE_NONE = 0x0,
        DEMO_TYPE_CLIENT = 0x1,
        DEMO_TYPE_SERVER = 0x2,
    };

    enum CubemapShot
    {
        CUBEMAPSHOT_NONE = 0x0,
        CUBEMAPSHOT_RIGHT = 0x1,
        CUBEMAPSHOT_LEFT = 0x2,
        CUBEMAPSHOT_BACK = 0x3,
        CUBEMAPSHOT_FRONT = 0x4,
        CUBEMAPSHOT_UP = 0x5,
        CUBEMAPSHOT_DOWN = 0x6,
        CUBEMAPSHOT_COUNT = 0x7,
    };

    struct snapshot_s
    {
        int snapFlags;
        int ping;
        int serverTime;
        playerState_s ps;
        int numEntities;
        int numClients;
        entityState_s entities[512];
        clientState_s clients[64];
        int serverCommandSequence;
    };

    struct playerEntity_t
    {
        float fLastWeaponPosFrac;
        int bPositionToADS;
        float vPositionLastOrg[3];
        float fLastIdleFactor;
        float vLastMoveOrg[3];
        float vLastMoveAng[3];
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
        float origin[3];
        float angles[3];
        byte pad[0x30];  // some fields here
    };

    struct centity_s
    {
        cpose_t pose;
        LerpEntityState currentState;
        entityState_s nextState;
        bool nextValid;
        bool bMuzzleFlash;
        bool bTrailMade;
        int previousEventSequence;
        int miscTime;
        float lightingOrigin[3];
        void* tree; // XAnimTree_s
    };

    struct score_t
    {
        byte pad[0x28];
    };

    struct viewDamage_t
    {
        int time;
        int duration;
        float yaw;
    };

    struct shellshock_t
    {
        byte pad[0x20];
    };

    struct $F6DFD6D87F75480A1EF1906639406DF5
    {
        int time;
        int duration;
    };

    struct animScriptData_t
    {
        byte pad[0x9A9D0];
    };

    struct $0867E0FC4F8157A276DAB76B1612E229
    {
        byte pad[0x10];
    };

    struct lerpFrame_t
    {
        float yawAngle;
        int yawing;
        float pitchAngle;
        int pitching;
        int animationNumber;
        void* animation;
        int animationTime;
        float oldFramePos[3];
        float animSpeedScale;
        int oldFrameSnapshotTime;
    };

    struct clientControllers_t
    {
        float angles[6][3];
        float tag_origin_angles[3];
        float tag_origin_offset[3];
    };

    struct __declspec(align(4)) clientInfo_t
    {
        int infoValid;
        int nextValid;
        int clientNum;
        char name[16];
        team_t team;
        team_t oldteam;
        int rank;
        int prestige;
        int perks;
        int score;
        int location;
        int health;
        char model[64];
        char attachModelNames[6][64];
        char attachTagNames[6][64];
        lerpFrame_t legs;
        lerpFrame_t torso;
        float lerpMoveDir;
        float lerpLean;
        float playerAngles[3];
        int leftHandGun;
        int dobjDirty;
        clientControllers_t control;
        unsigned int clientConditions[10][2];
        void* pXAnimTree;
        int iDObjWeapon;
        char weaponModel;
        int stanceTransitionTime;
        int turnAnimEndTime;
        char turnAnimType;
        int attachedVehEntNum;
        int attachedVehSlotIndex;
        bool hideWeapon;
        bool usingKnife;
    };

    struct bgs_t
    {
        animScriptData_t animScriptData;
        $0867E0FC4F8157A276DAB76B1612E229 generic_human;
        int time;
        int latestSnapshotTime;
        int frametime;
        int anim_user;
        XModel*(__cdecl* GetXModel)(const char*);
        void(__cdecl* CreateDObj)(void*, unsigned __int16, void*, int, int, void*);
        unsigned __int16(__cdecl* AttachWeapon)(void*, unsigned __int16, void*);
        DObj_s*(__cdecl* GetDObj)(int, int);
        void(__cdecl* SafeDObjFree)(int, int);
        void*(__cdecl* AllocXAnim)(int);
        clientInfo_t clientinfo[64];
    };

    struct cg_s
    {
        int clientNum;
        int localClientNum;
        DemoType demoType;
        CubemapShot cubemapShot;
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
        float refdefViewAngles[3];
        float lastVieworg[3];
        float swayViewAngles[3];
        float swayAngles[3];
        float swayOffset[3];
        int iEntityLastType[1024];
        XModel* pEntityLastXModel[1024];
        float zoomSensitivity;
        bool isLoading;
        char objectiveText[1024];
        char scriptMainMenu[256];
        int scoresRequestTime;
        int numScores;
        int teamScores[4];
        int teamPings[4];
        int teamPlayers[4];
        score_t scores[64];
        int scoreLimit;
        int showScores;
        int scoreFadeTime;
        int scoresTop;
        int scoresOffBottom;
        int scoresBottom;
        int drawHud;
        int crosshairClientNum;
        int crosshairClientLastTime;
        int crosshairClientStartTime;
        int identifyClientNum;
        int cursorHintIcon;
        int cursorHintTime;
        int cursorHintFade;
        int cursorHintString;
        int lastClipFlashTime;
        int invalidCmdHintType;
        int invalidCmdHintTime;
        int lastHealthPulseTime;
        int lastHealthLerpDelay;
        int lastHealthClient;
        float lastHealth;
        float healthOverlayFromAlpha;
        float healthOverlayToAlpha;
        int healthOverlayPulseTime;
        int healthOverlayPulseDuration;
        int healthOverlayPulsePhase;
        bool healthOverlayHurt;
        int healthOverlayLastHitTime;
        float healthOverlayOldHealth;
        int healthOverlayPulseIndex;
        int proneBlockedEndTime;
        int lastStance;
        int lastStanceChangeTime;
        int lastStanceFlashTime;
        int voiceTime;
        unsigned int weaponSelect;
        int weaponSelectTime;
        unsigned int weaponLatestPrimaryIdx;
        int prevViewmodelWeapon;
        int equippedOffHand;
        viewDamage_t viewDamage[8];
        int damageTime;
        float damageX;
        float damageY;
        float damageValue;
        float viewFade;
        int weapIdleTime;
        int nomarks;
        int v_dmg_time;
        float v_dmg_pitch;
        float v_dmg_roll;
        float fBobCycle;
        float xyspeed;
        float kickAVel[3];
        float kickAngles[3];
        float offsetAngles[3];
        float gunPitch;
        float gunYaw;
        float gunXOfs;
        float gunYOfs;
        float gunZOfs;
        float vGunOffset[3];
        float vGunSpeed[3];
        float viewModelAxis[4][3];
        float rumbleScale;
        float compassNorthYaw;
        float compassNorth[2];
        void* compassMapMaterial; // Material*
        float compassMapUpperLeft[2];
        float compassMapWorldSize[2];
        int compassFadeTime;
        int healthFadeTime;
        int ammoFadeTime;
        int stanceFadeTime;
        int sprintFadeTime;
        int offhandFadeTime;
        int offhandFlashTime;
        shellshock_t shellshock;
        $F6DFD6D87F75480A1EF1906639406DF5 testShock;
        int holdBreathTime;
        int holdBreathInTime;
        int holdBreathDelay;
        float holdBreathFrac;
        float radarProgress;
        float selectedLocation[2];
        SprintState sprintStates;
        int _unk01;
        int _unk02;
        bgs_t bgs;
        // ...
    };

    struct directory_t
    {
        char path[256];
        char gamedir[256];
    };

    struct iwd_t
    {
        char iwdFilename[256];
        char iwdBasename[256];
        char iwdGamename[256];
        char* handle;
        int checksum;
        int pure_checksum;
        volatile int hasOpenFile;
        int numfiles;
        char referenced;
        unsigned int hashSize;
        void** hashTable;
        void* buildBuffer;
    };

    struct searchpath_s
    {
        searchpath_s* next;
        iwd_t* iwd;
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

    /* 754 */
    struct $BFBB53559BEAC4289F32B924847E59CB
    {
        int stringCount;
        const char** strings;
    };

    /* 755 */
    struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
    {
        int min;
        int max;
    };

    /* 756 */
    struct $251C2428A496074035CACA7AAF3D55BD
    {
        float min;
        float max;
    };

    /* 757 */
    union DvarLimits
    {
        $BFBB53559BEAC4289F32B924847E59CB enumeration;
        $9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
        $251C2428A496074035CACA7AAF3D55BD value;
        $251C2428A496074035CACA7AAF3D55BD vector;
    };

    /* 758 */
    struct dvar_s
    {
        const char* name;
        const char* description;
        unsigned __int16 flags;
        char type;
        bool modified;
        DvarValue current;
        DvarValue latched;
        DvarValue reset;
        DvarLimits domain;
        bool(__cdecl* domainFunc)(dvar_s*, DvarValue);
        dvar_s* hashNext;
    };

    struct GfxWorldVertexData
    {
        void* vertices;  // GfxWorldVertex*
        IDirect3DVertexBuffer9* worldVb;
    };

    struct GfxWorldVertexLayerData
    {
        char* data;
        IDirect3DVertexBuffer9* layerVb;
    };

    struct SunLightParseParams
    {
        char name[64];
        float ambientScale;
        float ambientColor[3];
        float diffuseFraction;
        float sunLight;
        float sunColor[3];
        float diffuseColor[3];
        bool diffuseColorHasBeenSet;
        float angles[3];
    };

    struct GfxLight
    {
        char type;
        char canUseShadowMap;
        char unused[2];
        float color[3];
        float dir[3];
        float origin[3];
        float radius;
        float cosHalfFovOuter;
        float cosHalfFovInner;
        int exponent;
        unsigned int spotShadowIndex;
        void* def;
    };

    struct GfxWorld
    {
        const char* name;
        const char* baseName;
        int planeCount;
        int nodeCount;
        int indexCount;
        unsigned __int16* indices;
        int surfaceCount;
        int streamInfo;
        int skySurfCount;
        int* skyStartSurfs;
        void* skyImage;  // originally a GfxImage*
        char skySamplerState;
        unsigned int vertexCount;
        GfxWorldVertexData vd;
        unsigned int vertexLayerDataSize;
        GfxWorldVertexLayerData vld;
        SunLightParseParams sunParse;
        GfxLight* sunLight;
        float sunColorFromBsp[3];
        unsigned int sunPrimaryLightIndex;
        unsigned int primaryLightCount;
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
        // ...
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
        float origin[3];
        float angles[3];
        // ...
    };

    union qfile_gus
    {
        _iobuf* o;
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
        iwd_t* zipFile;
        int streamed;
        char name[256];
    };

    clientConnection_t* GetClientConnection();
    clientStatic_t* GetClientStatic();
    clientActive_t* GetClientActive();
    cgs_t* GetClientGlobalsStatic();
    cg_s* GetClientGlobals();
    WinMouseVars_t* GetMouseVars();
    GfxWorld* GetGfxWorld();
    clientUIActive_t* GetClientUIActives();
    centity_s* GetEntities();
    uint16_t* GetClientObjectMap();
    DObj_s* GetObjBuf();
}  // namespace IWXMVM::IW3::Structures
