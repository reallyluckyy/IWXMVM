#pragma once
#include "StdInclude.hpp"
#include "Utilities/Signatures.hpp"

namespace IWXMVM::T4::Signatures
{
    struct T4Addresses
    {
#define Sig IWXMVM::Signatures::Signature < IWXMVM::Signatures::SignatureImpl
#define Lambda IWXMVM::Signatures::Lambdas

        using GAType = IWXMVM::Signatures::GameAddressType;

        Sig("51 8D 90 ?? ?? ?? ?? 52 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 57", GAType::Code, 20,
            Lambda::FollowCodeFlow) > fopen;
        Sig("A1 ?? ?? ?? ?? 50 8D 53 28 57 8D 4C 24 14", GAType::Code, -5,
            Lambda::FollowCodeFlow) > AnglesToAxis;
        Sig("8B F0 8B F9 FF 15 ?? ?? ?? ?? 8A 06", GAType::Code, -8) > Cbuf_AddText;
        Sig("83 C7 10 8B 8E ?? ?? 00 00 3B 0D ?? ?? ?? 00", GAType::Code,
            15) > CG_AddPlayerSpriteDrawSurfs;  // killcam 'you' marker
        Sig("C3 F6 05 ?? ?? ?? 00 10", GAType::Code, 8) > CL_CGameRendering;  // hide class menus
        Sig("E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 85 C0 74 1D", GAType::Code, -5) > CG_CalcViewValues;  // first call to AnglesToAxis we need to replace
        Sig("56 57 8B F8 8D 44 24 0F 50 51 8B CF C6 44 24 17 FE E8 ?? ?? ?? ?? 83 C4 08 85 C0 74 25", GAType::Code, -6) > CG_DObjGetWorldTagMatrix;
        Sig("00 53 56 57 8B F0 0F 85", GAType::Code, -9) > CG_DrawDisconnect;
        Sig("8D 74 24 50 D9 5C 24 50", GAType::Code, 8) > CG_OffsetThirdPersonView;  // second call to AnglesToAxis we need to replace
        Sig("8B F0 83 C4 04 85 F6 8B C7 74 14", GAType::Code, -5, Lambda::FollowCodeFlow) > Dvar_FindMalleableVar;
        Sig("F3 0F 10 40 1C F3 0F 11 05 ?? ?? ?? ?? F3 0F 10 40 20", GAType::Code, -6) > FX_SetupCamera;
        Sig("6A 00 56 8B F8 E8 ?? ?? ?? ?? D9 47 1C", GAType::Code, -9) > R_SetViewParmsForScene;
        Sig("8B C6 59 C3 56 E8 ?? ?? ?? ?? 83 C4 04 ?? ?? ?? ?? ?? CC", GAType::Code, 13) > SV_Frame;
        Sig("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 80 3D", GAType::Data, 1, Lambda::DereferenceAddress) > clientConnection;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 0C 8D 04 7F", GAType::Data, 1,
            Lambda::DereferenceAddress) > clientStatic;
        Sig("05 ?? ?? ?? ?? B9 01 00 00 00 01 88 9C 6A", GAType::Data, 1,
            Lambda::DereferenceAddress) > clientActive;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 0C 68 C0 DD 0F 00", GAType::Data, 1,
            Lambda::DereferenceAddress) > clientGlobalsStatic;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 0C 68 C0 DD 0F 00", GAType::Data, 20, Lambda::DereferenceAddress) > clientGlobals;
        Sig("89 1D ?? ?? ?? ?? 5E 5F", GAType::Data, 2, Lambda::DereferenceAddress) > mouseVars;
        Sig("8B 1D ?? ?? ?? ?? 85 DB 74 E0", GAType::Data, 2) > fs_searchpaths;
        Sig("F8 83 EC 3C 53 56 57", GAType::Code, -5) > MainWndProc;
        Sig("C6 05 ?? ?? ?? ?? 01 88 9E", GAType::Data, 2, Lambda::DereferenceAddress) > clientUIActives;
        Sig("8B 54 24 24 53 55 56 57", GAType::Code, -7) > SL_GetStringOfSize;
        Sig("83 C4 0C 68 00 F0 0A 00 05", GAType::Data, 9, Lambda::DereferenceAddress) > cg_entities;
        Sig("56 8B F1 56 50 8B C7", GAType::Code, -6) > CG_DObjGetWorldBoneMatrix;
        Sig("03 44 24 04 0F B7 04 45", GAType::Data, 8, Lambda::DereferenceAddress) > clientObjMap;
        Sig("0F BF D8 6B DB 68 81", GAType::Data, 8, Lambda::DereferenceAddress) > objBuf;
        Sig("8B 94 24 20 04 00 00 80 BA ?? ?? ?? ?? 00", GAType::Data, -4) > CL_KeyEvent;
        Sig("A1 ?? ?? ?? ?? 8B 08 8B 51 68", GAType::Data, 1) > d3d9DevicePointer;

        // for rewinding
        Sig("8D 4C 24 0C 6A 04 51", GAType::Code, 7, Lambda::FollowCodeFlow) > FS_Read;
        Sig("89 86 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 20", GAType::Data, 2, Lambda::DereferenceAddress) > fsh;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 0C 88 1D", GAType::Data, 1,
            Lambda::DereferenceAddress) > lastValidBasepath;
        Sig("68 ?? ?? ?? ?? 89 55 00 E8", GAType::Data, 1, Lambda::DereferenceAddress) > s_compassActors;
        Sig("39 8C 07 ?? ?? ?? ?? 5F", GAType::Data, 3, Lambda::DereferenceAddress) > conGameMsgWindow0; // killfeed
        Sig("56 8B F0 75 7F E8", GAType::Code, -7) > CL_FirstSnapshot;
        Sig("8B 15 ?? ?? ?? ?? 8B 42 10 83 C4 04 80 38 00", GAType::Code, -5) > Con_TimeJumpedCall;
        Sig("8B C6 69 C0 D0 02 00 00 83 C4 08", GAType::Code, -5) > CG_MapRestartSetThirdpersonCall;

        // for depth patch
        Sig("56 57 33 FF 85 ED 74 48", GAType::Code, -5) > R_DoesDrawSurfListInfoNeedFloatz;

        // TODO: for changing (death) animations
        // Sig("E8 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 83 F8 0E 0F 87", GAType::Code, 5) > CG_ProcessEntity;

        Sig("55 8B 6C 24 38 85 ED", GAType::Code, -5) > R_AddCmdDrawTextWithEffects;
        Sig("E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 35 80 78 0A 00", GAType::Code, -6) > IN_Frame;

        Sig("8B 6C 24 2C 8B C5", GAType::Code, -5) > R_SetupMaterial;
        Sig("A1 ?? ?? ?? ?? C3 A1 ?? ?? ?? ?? 80 78 10 00", GAType::Code, -5) > Material_RegisterHandle;
        Sig("E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 66 0F EF C0", GAType::Data, -4, Lambda::DereferenceAddress) > rgp;

        Sig("83 C4 2C 5E 83 C4 20 C3", GAType::Code, -5) > CG_DrawPlayerLowHealthOverlay;
        Sig("0F 84 ?? ?? ?? ?? 8B 81 ?? ?? ?? 00 8B 91 ?? ?? ?? 00", GAType::Code, -7) > CG_DrawFlashDamage;
        Sig("53 55 56 57 0F 84 ?? ?? 00 00 8B 0D ?? ?? ?? ?? 2B 0D", GAType::Code, -10) > CG_DrawDamageDirectionIndicators;

        Sig("56 57 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 8B C3 74 08 E8", GAType::Code, -5) > Dvar_SetStringByName;
        Sig("3B C7 7D 52 8D 9B 00 00 00 00", GAType::Code, -6) > CG_ExecuteNewServerCommands;

        Sig("8B 04 8D ?? ?? ?? ?? 81 EC 24 01 00 00", GAType::Code, -6) > CL_PlayDemo_f;


#undef Sig
#undef Lambda
    };
}  // namespace IWXMVM::T4::Signatures
