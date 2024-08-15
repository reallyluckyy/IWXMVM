#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Utilities/HookManager.hpp"
#include "../Addresses.hpp"

namespace IWXMVM::IW5::Hooks::Camera
{
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
        HookManager::CreateHook(GetGameAddresses().CG_UpdateFov(), (std::uintptr_t)CG_UpdateFov_Hook,
                                (uintptr_t *)&CG_UpdateFov_Trampoline);
    }
}  // namespace IWXMVM::IW5::Hooks::Camera
