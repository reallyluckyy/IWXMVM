#pragma once
#include "StdInclude.hpp"
#include "Utilities/Signatures.hpp"

namespace IWXMVM::IW5::Signatures
{
    struct IW5Addresses
    {
#define Sig IWXMVM::Signatures::Signature < IWXMVM::Signatures::SignatureImpl
#define Lambda IWXMVM::Signatures::Lambdas

        using GAType = IWXMVM::Signatures::GameAddressType;

        Sig("83 C4 04 89 06 8B 0E", GAType::Data, -5, Lambda::FollowCodeFlow) > Dvar_FindDvar;
        Sig("8B 44 24 0C 8A 08 83 C4 04", GAType::Code, -7) > Cbuf_AddText;
        Sig("56 89 74 24 0C E8", GAType::Code, -8) > Com_TimeScaleMsec;

        Sig("8B F0 E8 ?? ?? ?? ?? 83 C4 14 84 C0", GAType::Data, -4, Lambda::DereferenceAddress) > clientGlobals;
        Sig("E8 ?? ?? ?? ?? 68 B8 4D 10 00 53", GAType::Data, -4, Lambda::DereferenceAddress) > clientGlobalsStatic;
        Sig("E8 ?? ?? ?? ?? 83 C4 04 84 C0 74 2A 8B 5C 24 20", GAType::Data, -4,
            Lambda::DereferenceAddress) > clientActive;
        Sig("53 51 E8 ?? ?? ?? ?? 33 C0", GAType::Data, -4, Lambda::DereferenceAddress) > cg_entities;
        Sig("83 3E 00 74 05", GAType::Data, -4, Lambda::DereferenceAddress) > clientInfo;
        Sig("75 19 68 ?? ?? ?? ?? 6A 00 C7 05", GAType::Data, -5, Lambda::DereferenceAddress) > clientStatic;
        Sig("E8 ?? ?? ?? ?? 8B 44 24 2C 50 C7 05", GAType::Data, -4, Lambda::DereferenceAddress) > cls_gameState;
        Sig("E8 ?? ?? ?? ?? 83 C4 04 5E 85 C0 75 08", GAType::Data, -4, Lambda::DereferenceAddress) > clientConnection;
        Sig("85 C0 74 0E 8B 80 70 E2 31 00", GAType::Data, -4, Lambda::DereferenceAddress) > s_clientDemoPlayback;
        Sig("68 38 0E 00 00", GAType::Data, 8, Lambda::DereferenceAddress) > s_compassActors;
        
        Sig("E8 ?? ?? ?? ?? 83 C4 04 84 C0 74 15 A1 ?? ?? ?? ?? 6A 00", GAType::Code, -5) > IN_Frame;
        Sig("56 E8 ?? ?? ?? ?? 83 C4 04 84 C0 0F 84 B9 04 00 00", GAType::Code, -6) > CL_Demo_HandleInput;
        Sig("83 EC 78 56 8B 75 08", GAType::Code, -6) > CL_Demo_FreeCameraMove;
        Sig("83 EC 34 53 56 8B 75 08 57", GAType::Code, -6) > CL_Demo_FollowCameraMove;
        Sig("E8 ?? ?? ?? ?? 83 C4 04 D9 E8 51", GAType::Code, -5) > CL_Demo_CalcViewValues_Location1;
        Sig("E8 ?? ?? ?? ?? 83 C4 04 D9 05 ?? ?? ?? ?? 55", GAType::Code, -5) > CL_Demo_CalcViewValues_Location2;
        Sig("33 C0 83 C4 40 89 44 24 60", GAType::Code, -5, Lambda::FollowCodeFlow) > CL_Demo_GetStartAndEndTime;
        Sig("8B 04 8D ?? ?? ?? ?? 81 EC 4C 02 00 00", GAType::Code, -6) > CL_Demo_Play;
        Sig("56 74 2A A1 ?? ?? ?? ?? 8B 0D", GAType::Code, -7) > CL_Demo_ReadInternal;
        Sig("74 46 8B 44 24 0C", GAType::Code, -8) > CL_Demo_SeekFile;
        Sig("68 ?? ?? ?? ?? D8 0D", GAType::Code, -7) > CG_UpdateFov;

        Sig("0F 85 94 00 00 00 53 E8", GAType::Code, -10) > CL_FirstSnapshot;
        Sig("56 E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 20 E8", GAType::Code, -5) > Con_TimeJumped_Call;
        Sig("56 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 4D", GAType::Code, -5) > CG_ScoresUp;
        Sig("D9 C0 D9 05 ?? ?? ?? ?? DD E1 DF E0 DD D9 D9 44 24 04", GAType::Data, 4,
            Lambda::DereferenceAddress) > com_codeTimeScale;
        Sig("56 8B 74 24 0C 3B C6 7D 1E", GAType::Code, -5) > CG_ExecuteNewServerCommands;

        Sig("74 0E 83 3D ?? ?? ?? ?? ?? 0F 95 C0", GAType::Code, -7) > CG_DrawShellshockBlend;
        Sig("50 E8 ?? ?? ?? ?? 83 C4 04 85 C0 0F 84 E9 00 00 00", GAType::Code, -5) > Item_GameMsgWindow_Paint;
        Sig("0F 84 32 01 00 00 8D 50 01", GAType::Code, -7) > R_AddCmdDrawText;
        Sig("85 F6 75 06 8B 35 ?? ?? ?? ?? 8B 46 54 8B 40 08 83 78 30 00 75 06 83 78 20 00 74 21", GAType::Code,
            -5) > R_AddCmdDrawTextWithEffects;
        Sig("83 C4 28 3B 35", GAType::Code, -5, Lambda::FollowCodeFlow) > R_AddCmdDrawStretchPic;
        Sig("74 39 80 3D ?? ?? ?? ?? ?? 74 1B", GAType::Code, -6) > CG_PainVisionUpdate_Location;

        Sig("56 8B 74 24 40 57 8B 7C 24 40", GAType::Code, -7) > CG_DObjGetWorldBoneMatrix;
        Sig("83 C4 08 85 C0 75 01 C3 8B 54 24 04 52", GAType::Code, -5,
            Lambda::FollowCodeFlow) > Com_GetClientDObj;
        Sig("56 8D 70 01 8D 9B 00 00 00 00 8A 08 40 84 C9 75 F9 2B C6 6A 07 40 50 6A 01", GAType::Code,
            -6) > Scr_AllocString;

        Sig("C6 05 ?? ?? ?? ?? ?? 8B 08 8B 91 A4 00 00 00", GAType::Data, -4) > d3d9DevicePointer;
        Sig("6A 02 FF 15 ?? ?? ?? ?? 8B 6C 24 48", GAType::Code, -5) > MainWndProc;

#undef Sig
#undef Lambda
    };
}  // namespace IWXMVM::IW5::Signatures
