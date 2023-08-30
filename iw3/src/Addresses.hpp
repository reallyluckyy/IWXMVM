#pragma once
#include "StdInclude.hpp"
#include "Signatures.hpp"
#include "Utilities/Signatures.hpp"

namespace IWXMVM::IW3
{
	struct IW3Addresses
	{
		IW3Addresses() = default;

		IW3Addresses(std::vector<HMODULE>& moduleHandles)
		{
			fopen							= sigfopen.Scan(moduleHandles);
			AnglesToAxis					= sigAnglesToAxis.Scan(moduleHandles);
			Cbuf_AddText					= sigCbuf_AddText.Scan(moduleHandles);
			CG_AddPlayerSpriteDrawSurfs		= sigCG_AddPlayerSpriteDrawSurfs.Scan(moduleHandles);
			CG_CalcViewValues				= sigCG_CalcViewValues.Scan(moduleHandles);
			CG_DObjGetWorldTagMatrix		= sigCG_DObjGetWorldTagMatrix.Scan(moduleHandles);
			CG_DrawConnectionInterrupted	= sigCG_DrawConnectionInterrupted.Scan(moduleHandles);
			CG_DrawTracer					= sigCG_DrawTracer.Scan(moduleHandles);
			CG_OffsetThirdPersonView		= sigCG_OffsetThirdPersonView.Scan(moduleHandles);
			CL_SystemInfoChanged			= sigCL_SystemInfoChanged.Scan(moduleHandles);
			CL_Vid_Restart_f				= sigCL_Vid_Restart_f.Scan(moduleHandles);
			Dvar_FindMalleable = sigDvar_FindMalleable.Scan(moduleHandles);
			FX_SetupCamera					= sigFX_SetupCamera.Scan(moduleHandles);
			R_SetViewParmsForScene			= sigR_SetViewParmsForScene.Scan(moduleHandles);
			SV_Frame						= sigSV_Frame.Scan(moduleHandles);

			if (cod4xDetected = (moduleHandles.size() > 1) ? true : false; cod4xDetected)
			{
				// search cod4x module first
				std::reverse(moduleHandles.begin(), moduleHandles.end());

				CL_SystemInfoChangedCoD4X = sigCL_SystemInfoChangedCoD4X.Scan(moduleHandles);
			}
		}

		std::uintptr_t fopen{};
		std::uintptr_t AnglesToAxis{};
		std::uintptr_t Cbuf_AddText{};
		std::uintptr_t CG_AddPlayerSpriteDrawSurfs{};
		std::uintptr_t CG_CalcViewValues{};
		std::uintptr_t CG_DObjGetWorldTagMatrix{};
		std::uintptr_t CG_DrawConnectionInterrupted{};
		std::uintptr_t CG_DrawTracer{};
		std::uintptr_t CG_OffsetThirdPersonView{};
		std::uintptr_t CL_SystemInfoChanged{};
		std::uintptr_t CL_Vid_Restart_f{};
		std::uintptr_t Dvar_FindMalleable{};
		std::uintptr_t FX_SetupCamera{};
		std::uintptr_t R_SetViewParmsForScene{};
		std::uintptr_t SV_Frame{};

		bool cod4xDetected{};
		std::uintptr_t CL_SystemInfoChangedCoD4X{};
	};
}