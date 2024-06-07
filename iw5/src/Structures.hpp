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

    struct clientConnection_t
    {
        uint32_t qport;
        int32_t clientNum;
        uint32_t lastPacketSendTime;
        uint32_t lastPacketTime;
        netadr_t serverAddress;
        uint32_t connectTime;
        uint32_t connectPacketCount;
        uint8_t serverMessage[256];
        uint32_t challenge;
        uint32_t checksumFeed;
        uint32_t reliableSequence;
        uint32_t reliableAcknowledge;
        uint8_t reliableCommands[128][1024];
        uint32_t serverMessageSequence;
        uint32_t serverCommandSequence;
        uint32_t lastExecutedServerCommand;
        uint8_t serverCommands[128][1024];
        uint32_t isServerRestarting;
        uint8_t demoName[256];
        uint32_t demoActive;
        uint32_t demoRecording;
        uint32_t demoState;
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

    clientConnection_t* GetClientConnection();
    clientDemoPlayback_t* GetClientDemoPlayback();
}  // namespace IWXMVM::IW5::Structures
