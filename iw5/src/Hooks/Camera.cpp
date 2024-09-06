#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Utilities/HookManager.hpp"
#include "Utilities/MathUtils.hpp"
#include "../Addresses.hpp"
#include "../Structures.hpp"

namespace IWXMVM::IW5::Hooks::Camera
{
    typedef void (*CL_Demo_CalcViewValues_t)();
    CL_Demo_CalcViewValues_t CL_Demo_CalcViewValues_Trampoline;
    void CL_Demo_CalcViewValues_Hook()
    {
        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            return;

        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        auto cg = Structures::GetClientGlobals();

        if (!camera->IsModControlledCameraMode())
        {
            camera->GetPosition() = *reinterpret_cast<glm::vec3*>(cg->refdef.view.org);
            // TODO: set camera rotation
            // camera->GetRotation() = MathUtils::AxisToAngles(cg->refdef.view.axis);
            camera->GetFov() = glm::degrees(std::atan(cg->refdef.view.tanHalfFovX) * 2.0f);
            return;
        }

        cg->refdef.view.org[0] = camera->GetPosition()[0];
        cg->refdef.view.org[1] = camera->GetPosition()[1];
        cg->refdef.view.org[2] = camera->GetPosition()[2];

        cg->refdef.view.zNear = 0.01f;

        const auto axis = MathUtils::AnglesToAxis(camera->GetRotation());
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                cg->refdef.view.axis[x][y] = axis[x][y];
            }
        }
    }

    typedef void (*CG_UpdateFov_t)(float);
    CG_UpdateFov_t CG_UpdateFov_Trampoline;
    void CG_UpdateFov_Hook(float fov)
    {
        if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
            return;

        auto& camera = Components::CameraManager::Get().GetActiveCamera();
        if (camera->IsModControlledCameraMode())
        {
            fov = camera->GetFov();
        }

        CG_UpdateFov_Trampoline(fov);
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CL_Demo_CalcViewValues(), (std::uintptr_t)CL_Demo_CalcViewValues_Hook,
								(uintptr_t *)&CL_Demo_CalcViewValues_Trampoline);

        HookManager::CreateHook(GetGameAddresses().CG_UpdateFov(), (std::uintptr_t)CG_UpdateFov_Hook,
                                (uintptr_t *)&CG_UpdateFov_Trampoline);
    }
}  // namespace IWXMVM::IW5::Hooks::Camera
