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

	enum he_type_t
	{
		HE_TYPE_FREE = 0x0,
		HE_TYPE_TEXT = 0x1,
		HE_TYPE_VALUE = 0x2,
		HE_TYPE_PLAYERNAME = 0x3,
		HE_TYPE_MAPNAME = 0x4,
		HE_TYPE_GAMETYPE = 0x5,
		HE_TYPE_MATERIAL = 0x6,
		HE_TYPE_TIMER_DOWN = 0x7,
		HE_TYPE_TIMER_UP = 0x8,
		HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
		HE_TYPE_TENTHS_TIMER_UP = 0xA,
		HE_TYPE_CLOCK_DOWN = 0xB,
		HE_TYPE_CLOCK_UP = 0xC,
		HE_TYPE_WAYPOINT = 0xD,
		HE_TYPE_COUNT = 0xE,
	};

	struct $C96EA5EC2ACBB9C0BF22693F316ACC67
	{
		char r;
		char g;
		char b;
		char a;
	};

	union hudelem_color_t
	{
		$C96EA5EC2ACBB9C0BF22693F316ACC67 s0;
		int rgba;
	};

	struct hudelem_s
	{
		he_type_t type;
		float x;
		float y;
		float z;
		int targetEntNum;
		float fontScale;
		int font;
		int alignOrg;
		int alignScreen;
		hudelem_color_t color;
		hudelem_color_t fromColor;
		int fadeStartTime;
		int fadeTime;
		int label;
		int width;
		int height;
		int materialIndex;
		int offscreenMaterialIdx;
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
		hudelem_color_t glowColor;
		int fxBirthTime;
		int fxLetterTime;
		int fxDecayStartTime;
		int fxDecayDuration;
		int soundID;
		int flags;
	};

	struct $15067B6A14D88D7E1E730369692C3A81
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
		$15067B6A14D88D7E1E730369692C3A81 hud;
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

	struct __declspec(align(4)) WinMouseVars_t
	{
		int oldButtonState;
		tagPOINT oldPos;
		bool mouseActive;
		bool mouseInitialized;
	};


	clientConnection_t* GetClientConnection();
	clientActive_t* GetClientActive();
	WinMouseVars_t* GetMouseVars();

	dvar_s* FindDvar(const std::string name);

	std::string GetFilePath(const std::string demoName);
}