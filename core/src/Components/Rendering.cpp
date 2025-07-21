#include "StdInclude.hpp"
#include "Rendering.hpp"

#include "Mod.hpp"

namespace IWXMVM::Components::Rendering
{
    std::atomic<Types::RenderingFlags> renderingFlags;

    void SetGreenscreenColor(uint32_t color)
    {
        auto r_clearcolor = Mod::GetGameInterface()->GetDvar("r_clearcolor").value();
        r_clearcolor.value->uint32 = color;

        auto r_clear = Mod::GetGameInterface()->GetDvar("r_clear").value();
        r_clear.value->int32 = 3; // 3 = "steady" - clear every frame with the specified color
    }

    void SetDrawGun(bool drawGun)
    {
		auto cg_drawgun = Mod::GetGameInterface()->GetDvar("cg_drawgun").value();
		cg_drawgun.value->int32 = drawGun;
	}

    Types::RenderingFlags GetRenderingFlags()
    {
        return renderingFlags;
    }

    void SetRenderingFlags(Types::RenderingFlags flags)
    {
        renderingFlags = flags;
    }

    void SetVisibleElements(VisibleElements elements)
    {
        if (elements == VisibleElements::Everything)
        {
            ResetVisibleElements();
			return;
		}

        SetGreenscreenColor(0xFF00FF00);

        if (elements == VisibleElements::WorldAndPlayers)
        {
			SetRenderingFlags(Types::RenderingFlags_DrawEverything);
            SetDrawGun(0);
		} 
        else if (elements == VisibleElements::OnlyGun)
        {
            SetDrawGun(1);

            // The visuals for this will be drawn via a shader, 
            // see Graphics/Graphics.cpp
        }
        else if (elements == VisibleElements::OnlyWorld)
        {
			SetRenderingFlags(Types::RenderingFlags_DrawWorld);
            SetDrawGun(0);
		}
        else if (elements == VisibleElements::OnlyPlayers)
        {
            SetRenderingFlags(Types::RenderingFlags_DrawPlayers);
            SetDrawGun(0);
        }
        else
        {
			LOG_WARN("Unknown pass elements: {0}", magic_enum::enum_name(elements));
		}
    }

    void ResetVisibleElements()
    {
        SetRenderingFlags(Types::RenderingFlags_DrawEverything);
        SetDrawGun(1);
        SetGreenscreenColor(0xFF000000);
    }

    void Initialize()
    {
        renderingFlags = Types::RenderingFlags_DrawEverything;
    }
}  // namespace IWXMVM::Components::Rendering