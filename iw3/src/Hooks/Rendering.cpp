#include "StdInclude.hpp"
#include "Rendering.hpp"

#include "Utilities/HookManager.hpp"
#include "../Structures.hpp"
#include "../Addresses.hpp"
#include "../Functions.hpp"

namespace IWXMVM::IW3::Hooks::Rendering
{
    const char *playerMaterials[] = {"weapon",
                                     "wpn",
                                     "gmz",
                                     "scope_overlay",
                                     "mc/mtl_usmc",
                                     "mc/mtl_arab",
                                     "mc/mtl_opforce",
                                     "mc/mtl_op_force",
                                     "mc/mtl_sas",
                                     "mc/mtl_desert_",
                                     "mc/mtl_marine",
                                     "mc/mtl_wpn",
                                     "mc/mtl_ghillie",
                                     "mc/mtl_head",
                                     "mc/mtl_bodies",
                                     "accessories",
                                     "eye"};
    const char *muzzleFlashAndTracersMaterials[] = {"flash", "trace"};

    bool IsMaterialOneOf(const char *materialName, const auto &materials)
    {
        for (const auto &s : materials)
        {
            if (strstr(materialName, s) != 0)
            {
                return true;
            }
        }

        return false;
    }

    void ReplaceMaterial(Structures::GfxDrawSurf &drawSurf, Structures::Material *newMaterial)
    {
        drawSurf.fields.materialSortedIndex &= ~(0x7FF);
        drawSurf.fields.materialSortedIndex |= (newMaterial->info.drawSurf.fields.materialSortedIndex & 0x7FF);
    }

    typedef int32_t(__cdecl *R_SetupMaterial_t)(Structures::GfxCmdBufSourceState *a1, void *a2,
                                                Structures::GfxCmdBufSourceState *src,
                                                Structures::GfxCmdBufState *state, uint32_t drawSurfLow,
                                                uint32_t drawSurfHigh);
    R_SetupMaterial_t R_SetupMaterial_Trampoline;

    int32_t R_SetupMaterial_Hook(Structures::GfxCmdBufSourceState *a1, void *a2, Structures::GfxCmdBufSourceState *src,
                                 Structures::GfxCmdBufState *state, uint32_t drawSurfLow, uint32_t drawSurfHigh)
    {
        Structures::GfxDrawSurf drawSurf;
        drawSurf.packed = ((uint64_t)(drawSurfHigh) << 32) | (uint32_t)(drawSurfLow);

        auto rgp = reinterpret_cast<Structures::r_global_permanent_t *>(GetGameAddresses().rgp());
        auto material = rgp->sortedMaterials[drawSurf.fields.materialSortedIndex & 0x7FF];

        auto renderingFlags = Components::Rendering::GetRenderingFlags();

        if ((renderingFlags & Types::RenderingFlags_DrawPlayers) == 0 &&
            IsMaterialOneOf(material->info.name, playerMaterials))
        {
            ReplaceMaterial(drawSurf, Functions::Material_RegisterHandle("overlay_low_health"));
        }
        if ((renderingFlags & Types::RenderingFlags_DrawMuzzleFlash) == 0 &&
            IsMaterialOneOf(material->info.name, muzzleFlashAndTracersMaterials))
        {
            ReplaceMaterial(drawSurf, Functions::Material_RegisterHandle("overlay_low_health"));
        }

        bool isWorldMaterial = !IsMaterialOneOf(material->info.name, playerMaterials) &&
                               !IsMaterialOneOf(material->info.name, muzzleFlashAndTracersMaterials);
        if ((renderingFlags & Types::RenderingFlags_DrawWorld) == 0 && isWorldMaterial)
        {
            ReplaceMaterial(drawSurf, Functions::Material_RegisterHandle("overlay_low_health"));
        }

        return R_SetupMaterial_Trampoline(a1, a2, src, state, drawSurf.packed & 0xFFFFFFFF,
                                          (drawSurf.packed >> 32) & 0xFFFFFFFF);
    }

    void Install()
    {
        HookManager::CreateHook(GetGameAddresses().R_SetupMaterial(), (std::uintptr_t)R_SetupMaterial_Hook,
                                (uintptr_t *)&R_SetupMaterial_Trampoline);
    }
}  // namespace IWXMVM::IW3::Hooks::Rendering