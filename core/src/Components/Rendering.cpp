#include "StdInclude.hpp"
#include "Rendering.hpp"

#include "Mod.hpp"

namespace IWXMVM::Components::Rendering
{
    std::atomic<Types::RenderingFlags> renderingFlags;

    void SetGreenscreenColor()
    {
        auto r_clearcolor = Mod::GetGameInterface()->GetDvar("r_clearcolor").value();
        r_clearcolor.value->uint32 = 0xFF00FF00;
    }

    Types::RenderingFlags GetRenderingFlags()
    {
        return renderingFlags;
    }

    void SetRenderingFlags(Types::RenderingFlags flags)
    {
        renderingFlags = flags;

        // Since we cant hide the viewmodel via the R_SetMaterial, we set the cg_drawgun
		auto cg_drawgun = Mod::GetGameInterface()->GetDvar("cg_drawgun").value();
        cg_drawgun.value->int32 = flags & Types::RenderingFlags_DrawViewmodel ? 1 : 0;

        if (flags != Types::RenderingFlags_DrawEverything)
		{
			SetGreenscreenColor();
		}
    }

    void Initialize()
    {
        renderingFlags = Types::RenderingFlags_DrawEverything;
    }
}  // namespace IWXMVM::Components::Rendering