#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Utilities/HookManager.hpp"
#include "Utilities/MathUtils.hpp"
#include "../Structures.hpp"
#include "../Functions.hpp"
#include "../Addresses.hpp"
#include "Mod.hpp"

namespace IWXMVM::IW3::Hooks::Camera
{

    void R_SetViewParmsForScene()
    {
        auto& refdef = Structures::GetClientGlobals()->refdef;

        auto& camera = Components::CameraManager::Get().GetActiveCamera();

        if (!camera->IsModControlledCameraMode())
        {
            camera->GetPosition() = *reinterpret_cast<glm::vec3*>(refdef.vieworg);
            camera->GetFov() = glm::degrees(std::atan(refdef.tanHalfFovX) * 2.0f);
            return;
        }

        refdef.vieworg[0] = camera->GetPosition()[0];
        refdef.vieworg[1] = camera->GetPosition()[1];
        refdef.vieworg[2] = camera->GetPosition()[2];

        refdef.tanHalfFovX = std::tan(glm::radians(camera->GetFov()) * 0.5f);
        refdef.tanHalfFovY = refdef.tanHalfFovX * ((float)refdef.height / (float)refdef.width);
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
        auto& camera = Components::CameraManager::Get().GetActiveCamera();

        if (!camera->IsModControlledCameraMode())
        {
            camera->GetRotation() = *reinterpret_cast<glm::vec3*>(angles);
            return;
        }

        angles[0] = camera->GetRotation()[0];
        angles[1] = camera->GetRotation()[1];
        angles[2] = camera->GetRotation()[2];
    }

    uintptr_t AnglesToAxis_Address;
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
        auto& camera = Components::CameraManager::Get().GetActiveCamera();

        if (!camera->IsModControlledCameraMode())
            return;

        auto& refdef = Structures::GetClientGlobals()->refdef;
        refdef.vieworg[0] = camera->GetPosition()[0];
        refdef.vieworg[1] = camera->GetPosition()[1];
        refdef.vieworg[2] = camera->GetPosition()[2];
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
            if (Components::CameraManager::Get().GetActiveCamera()->IsModControlledCameraMode() &&
                Components::CameraManager::Get().GetActiveCamera()->GetMode() != Components::Camera::Mode::Bone)
                tempEDI = dummyViewAxis;
        }

        __asm popad
        __asm mov edi, tempEDI 
        __asm jmp CG_DObjGetWorldTagMatrix_Trampoline
    }

    void Install()
    {
        // rewrite the camera position and fov
        HookManager::CreateHook(GetGameAddresses().R_SetViewParmsForScene(), (uintptr_t)R_SetViewParmsForScene_Hook,
                                &R_SetViewParmsForScene_Trampoline);

        // rewrite the camera angles
        AnglesToAxis_Address = GetGameAddresses().AnglesToAxis();
        HookManager::WriteCall(GetGameAddresses().CG_CalcViewValues(), (uintptr_t)AnglesToAxis_Hook);
        HookManager::WriteCall(GetGameAddresses().CG_OffsetThirdPersonView(), (uintptr_t)AnglesToAxis_Hook);

        // update position of world-space effects (such as smoke) with our new position
        HookManager::CreateHook(GetGameAddresses().FX_SetupCamera(), (uintptr_t)FX_SetupCamera_Hook,
                                &FX_SetupCamera_Trampoline);

        // TODO: CG_CalcFov
        // TODO: bypass connection interrupted (CI) image / message by placing a return statement at 0x42F930

        // ignore writes to camera angles (this fixes things like the player knifing affecting the freecam)
        HookManager::CreateHook(GetGameAddresses().CG_DObjGetWorldTagMatrix(), (uintptr_t)CG_DObjGetWorldTagMatrix_Hook,
                                &CG_DObjGetWorldTagMatrix_Trampoline);
    }

    void OnCameraChanged()
    {
        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        auto isFreeCamera = camera->IsModControlledCameraMode();

        Functions::FindDvar("cg_thirdperson")->current.enabled =
            (camera->GetMode() == Components::Camera::Mode::ThirdPerson || isFreeCamera) ? 1 : 0;
        Functions::FindDvar("cg_draw2d")->current.enabled = (isFreeCamera) ? 0 : 1;
        Functions::FindDvar("cg_drawShellshock")->current.enabled = (isFreeCamera) ? 0 : 1;

        constexpr int32_t LODBIAS = -40000;
        Functions::FindDvar("r_lodBiasRigid")->current.value = LODBIAS;
        Functions::FindDvar("r_lodBiasSkinned")->current.value = LODBIAS;
    }
}  // namespace IWXMVM::IW3::Hooks::Camera
