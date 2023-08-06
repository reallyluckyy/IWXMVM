#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Utilities/HookManager.hpp"
#include "Utilities/MathUtils.hpp"
#include "../Structures.hpp"
#include "Mod.hpp"

namespace IWXMVM::IW3::Hooks::Camera
{

	void R_SetViewParmsForScene() 
	{
		auto& camera = Mod::GetCameraManager()->GetActiveCamera();

		if (!camera.IsModControlledCameraMode())
		{
			// TODO: if we want to use the first/third person cameras position in core, 
			// TODO: we'd have to update it here with the values from refdef
			return;
		}

		auto refdef = Structures::GetRefDef();
		refdef->Origin[0] = camera.GetPosition()[0];
		refdef->Origin[1] = camera.GetPosition()[1];
		refdef->Origin[2] = camera.GetPosition()[2];

		refdef->FOV[0] = std::tan(MathUtils::DegreesToRadians(camera.GetFov()) * 0.5f);
		refdef->FOV[1] = refdef->FOV[0] * ((float)refdef->ScreenHeight / (float)refdef->ScreenWidth);
	}

	uintptr_t R_SetViewParmsForScene_Trampoline;
	void __declspec(naked) R_SetViewParmsForScene_Hook()
	{
		__asm pushad

		R_SetViewParmsForScene();

		__asm popad
		__asm jmp R_SetViewParmsForScene_Trampoline
	}

	void AnglesToAxis(float* angles) 
	{
		auto& camera = Mod::GetCameraManager()->GetActiveCamera();

		if (!camera.IsModControlledCameraMode())
			return;

		angles[0] = camera.GetRotation()[0];
		angles[1] = camera.GetRotation()[1];
		angles[2] = camera.GetRotation()[2];
	}

	uintptr_t AnglesToAxis_Address = 0x5635C0;
	void __declspec(naked) AnglesToAxis_Hook()
	{
		static float* angles;

		__asm pushad
		__asm mov angles, esi

		AnglesToAxis(angles);

		__asm popad
		__asm jmp AnglesToAxis_Address
	}

	void FX_SetupCamera() 
	{
		auto& camera = Mod::GetCameraManager()->GetActiveCamera();

		if (!camera.IsModControlledCameraMode())
			return;

		auto refdef = Structures::GetRefDef();
		refdef->Origin[0] = camera.GetPosition()[0];
		refdef->Origin[1] = camera.GetPosition()[1];
		refdef->Origin[2] = camera.GetPosition()[2];
	}

	uintptr_t FX_SetupCamera_Trampoline;
	void __declspec(naked) FX_SetupCamera_Hook()
	{
		__asm pushad

		FX_SetupCamera();

		__asm popad
		__asm jmp FX_SetupCamera_Trampoline
	}

	uint32_t CG_DObjGetWorldTagMatrix_Trampoline;
	void __declspec(naked) CG_DObjGetWorldTagMatrix_Hook()
	{
		static float* tempEDI;
		static float dummyViewAxis[9];

		__asm mov tempEDI, edi
		__asm pushad

		{
			if (Mod::GetCameraManager()->GetActiveCamera().IsModControlledCameraMode())
				tempEDI = dummyViewAxis;
		}

		__asm popad
		__asm mov edi, tempEDI
		__asm jmp CG_DObjGetWorldTagMatrix_Trampoline
	}

	void Install()
	{
		// rewrite the camera position and fov
		HookManager::CreateHook(0x5FA900, (uintptr_t)R_SetViewParmsForScene_Hook, &R_SetViewParmsForScene_Trampoline);

		// rewrite the camera angles
		HookManager::WriteCall(0x451C8D, (uintptr_t)AnglesToAxis_Hook);
		HookManager::WriteCall(0x44FD8C, (uintptr_t)AnglesToAxis_Hook);

		// update position of world-space effects (such as smoke) with our new position
		HookManager::CreateHook(0x4A56F0, (uintptr_t)FX_SetupCamera_Hook, &FX_SetupCamera_Trampoline);
		
		// TODO: CG_CalcFov

		// ignore writes to camera angles (this fixes things like the player knifing affecting the freecam)
		HookManager::CreateHook(0x434070, (uintptr_t)CG_DObjGetWorldTagMatrix_Hook, &CG_DObjGetWorldTagMatrix_Trampoline);
	}

	void OnCameraChanged()
	{
		auto& camera = Mod::GetCameraManager()->GetActiveCamera();
		auto isFreeCamera = camera.IsModControlledCameraMode();

		Structures::FindDvar("cg_thirdperson")->current.enabled = (isFreeCamera) ? 1 : 0;
		Structures::FindDvar("cg_draw2d")->current.enabled = (isFreeCamera) ? 0 : 1;

		Structures::FindDvar("r_lodBiasRigid")->current.value = -40000;
		Structures::FindDvar("r_lodBiasSkinned")->current.value = -40000;
	}
}