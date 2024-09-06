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

        cg->refdef.view.zNear = 1.0f;

        const auto aspect = ((float)cg->refdef.displayViewport.width / (float)cg->refdef.displayViewport.height);
        cg->refdef.view.tanHalfFovX = std::tan(glm::radians(camera->GetFov()) * 0.5f);
        cg->refdef.view.tanHalfFovY =
            cg->refdef.view.tanHalfFovX / aspect;

        const auto axis = MathUtils::AnglesToAxis(camera->GetRotation());
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                cg->refdef.view.axis[x][y] = axis[x][y];
            }
        }
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().CL_Demo_CalcViewValues(), (std::uintptr_t)CL_Demo_CalcViewValues_Hook,
								(uintptr_t *)&CL_Demo_CalcViewValues_Trampoline);
    }
}  // namespace IWXMVM::IW5::Hooks::Camera
