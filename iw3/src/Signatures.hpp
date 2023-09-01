#pragma once
#include "StdInclude.hpp"
#include "Utilities/Signatures.hpp"

namespace IWXMVM::IW3::Signatures
{
	inline auto IsAbsoluteJumpOrCall = [](std::uintptr_t address)
	{
		static constexpr std::uint8_t JUMP_OPCODE = 0xE9;
		static constexpr std::uint8_t CALL_OPCODE = 0xE8;

		if (*reinterpret_cast<std::uint8_t*>(address) == JUMP_OPCODE || *reinterpret_cast<std::uint8_t*>(address) == CALL_OPCODE)
			return true;
		else
			return false;
	};

	inline auto DereferenceAddress = [](std::uintptr_t address)
	{
		return *reinterpret_cast<std::uintptr_t*>(address + 1) + 5;
	};

	inline auto FollowCodeFlow = [](std::uintptr_t address)
	{
		const std::uintptr_t orgAddress = address;

		while (IsAbsoluteJumpOrCall(address))
			address += DereferenceAddress(address);

		return (address == orgAddress) ? 0 : address;
	};

	struct IW3Addresses
	{
#define Sig IWXMVM::Signatures::Signature<IWXMVM::Signatures::SignatureImpl
			
		using Type = IWXMVM::Signatures::GameAddressType;

		Sig("51 8D 90 ?? ?? ?? ?? 52 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 57",									Type::Code, 20, FollowCodeFlow)> fopen;
		Sig("53 8D 4C 24 ?? E8 ?? ?? ?? ?? 8D 54 24 ?? 8D 74 24 ?? 8B D8",									Type::Code, 20, FollowCodeFlow)> AnglesToAxis;
		Sig("8B F0 8B F9 FF 15 ?? ?? ?? ?? 8A 06",															Type::Code, -8)> Cbuf_AddText;
		Sig("8B F8 83 C7 ?? 8B 8E ?? ?? ?? ?? 3B 0D ?? ?? ?? ?? 75 ?? 83 3D ?? ?? ?? ?? 00",				Type::Code, 26)> CG_AddPlayerSpriteDrawSurfs; // killcam 'you' marker
		Sig("E8 ?? ?? ?? ?? 83 3D ?? ?? ?? ?? 00 74 ?? 68 ?? ?? ?? ?? E8",									Type::Code, -5)> CG_CalcViewValues; // first call to AnglesToAxis we need to replace
		Sig("5C 24 ?? 55 8B 6C 24 ?? 56 8D 44 24 ?? 50 51 8B CB C6 44 24",									Type::Code, -5)> CG_DObjGetWorldTagMatrix;
		Sig("00 53 56 57 8B F0 0F 85 ?? ?? ?? ?? 8D 44 24",													Type::Code, -9)> CG_DrawDisconnect;
		Sig("6B FF ?? 81 C7 ?? ?? ?? ?? ?? ?? ?? ?? ?? 8D 74 24 ?? F3 A5 83 05 ?? ?? ?? ?? 01",				Type::Code, 9)> CG_DrawTracer;
		Sig("8D 74 24 ?? D9 5C 24 ?? ?? ?? ?? ?? ?? 5F 5E 5B 8B E5 5D C3",									Type::Code, 8)> CG_OffsetThirdPersonView; // second call to AnglesToAxis we need to replace
		Sig("53 55 56 8B F0 05 ?? ?? ?? ?? 8B C8 57 8B",													Type::Code, -8)> CL_SystemInfoChanged;
		Sig("81 EC ?? ?? 00 00 A1 ?? ?? ?? ?? 53 33 DB 39",													Type::Code, -6)> CL_Vid_Restart_f;
		Sig("85 C0 74 ?? 8B FE E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? D9 41 ?? D8 4C 24 0C D9 5E 0C 5F 5E C3",	Type::Code, -5, FollowCodeFlow)> Dvar_FindMalleable;
		Sig("83 EC ?? D9 46 ?? D9 1D ?? ?? ?? ?? D9 46 ?? D9 1D",											Type::Code, -6)> FX_SetupCamera;
		Sig("8B F8 6A 00 57 E8 ?? ?? ?? ?? D9 46 ?? D9 9F",													Type::Code, -7)> R_SetViewParmsForScene;
		Sig("8B C6 59 C3 56 E8 ?? ?? ?? ?? 83 C4 04 ?? ?? ?? ?? ?? CC",										Type::Code, 13)> SV_Frame;

		// cod4x
		Sig("00 00 E8 ?? ?? ?? ?? 29 C4 C7 04 24 01 00 00 00 E8",											Type::Code, -7), true> CL_SystemInfoChangedCoD4X;

#undef Sig
	};
}