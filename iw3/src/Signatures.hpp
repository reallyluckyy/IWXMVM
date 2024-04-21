#pragma once
#include "StdInclude.hpp"
#include "Utilities/Signatures.hpp"

namespace IWXMVM::IW3::Signatures
{
    struct IW3Addresses
    {
#define Sig IWXMVM::Signatures::Signature < IWXMVM::Signatures::SignatureImpl
#define Lambda IWXMVM::Signatures::Lambdas

        using GAType = IWXMVM::Signatures::GameAddressType;

        Sig("51 8D 90 ?? ?? ?? ?? 52 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 57", GAType::Code, 20,
            Lambda::FollowCodeFlow) > fopen;
        Sig("53 8D 4C 24 ?? E8 ?? ?? ?? ?? 8D 54 24 ?? 8D 74 24 ?? 8B D8", GAType::Code, 20,
            Lambda::FollowCodeFlow) > AnglesToAxis;
        Sig("8B F0 8B F9 FF 15 ?? ?? ?? ?? 8A 06", GAType::Code, -8) > Cbuf_AddText;
        Sig("83 C7 10 8B 8E ?? 00 00 00 3B 0D ?? ?? ?? 00", GAType::Code,
            15) > CG_AddPlayerSpriteDrawSurfs;  // killcam 'you' marker
        Sig("C3 F6 05 ?? ?? ?? 00 10", GAType::Code, 8) > CL_CGameRendering;  // hide class menus
        Sig("8B C5 E8 ?? ?? ?? ?? BA", GAType::Code, 17) > CG_CalcViewValues;  // first call to AnglesToAxis we need to replace
        Sig("5C 24 ?? 55 8B 6C 24 ?? 56 8D 44 24 ?? 50 51 8B CB C6 44 24", GAType::Code, -5) > CG_DObjGetWorldTagMatrix;
        Sig("00 53 56 57 8B F0 0F 85 ?? ?? ?? ?? 8D 44 24", GAType::Code, -9) > CG_DrawDisconnect;
        Sig("8D 74 24 ?? D9 5C 24 ?? ?? ?? ?? ?? ?? 5F 5E 5B 8B E5 5D C3", GAType::Code,
            8) > CG_OffsetThirdPersonView;  // second call to AnglesToAxis we need to replace
        Sig("85 C0 74 ?? 8B FE E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? D9 41 ?? D8 4C 24 0C D9 5E 0C 5F 5E C3", GAType::Code,
            -5, Lambda::FollowCodeFlow) > Dvar_FindMalleableVar;
        Sig("83 EC ?? D9 46 ?? D9 1D ?? ?? ?? ?? D9 46 ?? D9 1D", GAType::Code, -6) > FX_SetupCamera;
        Sig("8B F8 6A 00 57 E8 ?? ?? ?? ?? D9 46 ?? D9 9F", GAType::Code, -7) > R_SetViewParmsForScene;
        Sig("8B C6 59 C3 56 E8 ?? ?? ?? ?? 83 C4 04 ?? ?? ?? ?? ?? CC", GAType::Code, 13) > SV_Frame;
        Sig("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 80 3D", GAType::Data, 1, Lambda::DereferenceAddress) > clientConnection;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 0C 68 ?? ?? ?? ?? C1 E6 04", GAType::Data, 1,
            Lambda::DereferenceAddress) > clientStatic;
        Sig("05 ?? ?? ?? ?? B9 01 00 00 00 01 88 B8 56 02 00", GAType::Data, 1,
            Lambda::DereferenceAddress) > clientActive;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 0C 68 F0 E7 0F 00", GAType::Data, 1,
            Lambda::DereferenceAddress) > clientGlobalsStatic;
        Sig("BA ?? ?? ?? ?? E8 ?? ?? ?? ?? D9 03", GAType::Data, 1, Lambda::DereferenceAddress) > clientGlobals;
        Sig("89 1D ?? ?? ?? ?? 5E 5F", GAType::Data, 2, Lambda::DereferenceAddress) > mouseVars;
        Sig("8B 1D ?? ?? ?? ?? 85 DB 74 E0", GAType::Data, 2) > fs_searchpaths;
        Sig("F8 83 EC 3C 53 56 57", GAType::Code, -5) > MainWndProc;
        Sig("?? ?? ?? ?? ?? 81 EC ?? ?? ?? ?? 8D 80 ?? ?? ?? ?? 8D 54 24 ?? 56", GAType::Code, 22) > CG_RegisterItems;
        Sig("C6 05 ?? ?? ?? ?? 01 88 9E", GAType::Data, 2, Lambda::DereferenceAddress) > clientUIActives;
        Sig("5C 24 20 55 56 8B 74 24 20", GAType::Code, -5) > SL_GetStringOfSize;
        Sig("69 C9 00 70 07 00 83 C4 0C 68 00 70 07 00 81", GAType::Data, 16, Lambda::DereferenceAddress) > cg_entities;
        Sig("5C 24 38 55 8B 6C 24 40 57 8B F9", GAType::Code, -5) > CG_DObjGetWorldBoneMatrix;
        Sig("03 44 24 04 0F B7 04 45", GAType::Data, 8, Lambda::DereferenceAddress) > clientObjMap;
        Sig("0F BF F0 6B F6 64 81 C6", GAType::Data, 8, Lambda::DereferenceAddress) > objBuf;
        Sig("0F B6 80 ?? ?? ?? ?? FF 24 85 ?? ?? ?? ?? 83 3D ?? ?? ?? ?? 00 75", GAType::Code, 22) > CL_KeyEvent;
        Sig("8B 0D ?? ?? ?? ?? 8B 40 04 8B 11 83 C6 04", GAType::Data, 2) > d3d9DevicePointer;

        // for rewinding
        Sig("83 C4 ?? 53 57 56 ?? ?? ?? ?? ?? 83 C4", GAType::Code, 6, Lambda::FollowCodeFlow) > FS_Read;
        Sig("89 86 ?? ?? ?? ?? E8 ?? ?? ?? ?? 88 9F FF 00 00 00", GAType::Data, 2, Lambda::DereferenceAddress) > fsh;
        Sig("68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 0C 88 1D", GAType::Data, 1,
            Lambda::DereferenceAddress) > lastValidBasepath;
        Sig("68 ?? ?? ?? ?? 89 1E", GAType::Data, 1, Lambda::DereferenceAddress) > s_compassActors;
        Sig("39 8C 07 ?? ?? ?? ?? 5F", GAType::Data, 3, Lambda::DereferenceAddress) > conGameMsgWindow0; // killfeed
        Sig("83 3D ?? ?? ?? ?? ?? 0F 85 92 01 00 00", GAType::Code, -5,
            Lambda::FollowCodeFlow) > CL_FirstSnapshot;
        Sig("8B 15 ?? ?? ?? ?? 8B 42 0C 83 C4 04 80 38 00", GAType::Code, -5) > Con_TimeJumpedCall;
        //Sig("EB 0F 68 ?? ?? ?? ?? 6A 01 E8 ?? ?? ?? ?? 83 C4 08 83 3D", GAType::Code, -9) > CL_SetCGameTimeError;
        //Sig("6A 01 E8 ?? ?? ?? ?? 83 C4 08 8B 15 ?? ?? ?? ?? 8B FE", GAType::Code,
            //-7) > CL_CGameNeedsServerCommandError;
        //Sig("6A 01 E8 ?? ?? ?? ?? 83 C4 08 57 53", GAType::Code, -7) > CG_ProcessSnapshotsError;
        //Sig("6A 0E E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? A1", GAType::Code, -9) > CG_ReadNextSnapshotWarning;
        Sig("8B C7 69 C0 58 02 00 00", GAType::Code, -5) > CG_MapRestartSetThirdpersonCall;

        // for changing (death) animations
        Sig("E8 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 83 F8 0E 0F 87", GAType::Code, 5) > CG_ProcessEntity;

        Sig("55 8B 6C 24 38 85 ED", GAType::Code, -5) > R_AddCmdDrawTextWithEffects;
        Sig("83 3D ?? ?? ?? ?? 09 75 ?? ?? ?? ?? ?? ?? 8B CD", GAType::Code, 9, Lambda::FollowCodeFlow) > IN_Frame;

        Sig("51 08 53 8B 5C", GAType::Code, -5) > R_SetupMaterial;
        Sig("A1 ?? ?? ?? ?? C3 A1 ?? ?? ?? ?? 80 78 0C 00", GAType::Code, -5) > Material_RegisterHandle;
        Sig("E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 33 C0 A3", GAType::Data, -4, Lambda::DereferenceAddress) > rgp;


#undef Sig
#undef Lambda
    };
}  // namespace IWXMVM::IW3::Signatures
