#include "StdInclude.hpp"
#include "Rendering.hpp"

namespace IWXMVM::Components::Rendering
{
    std::atomic<Types::RenderingFlags> renderingFlags;

    Types::RenderingFlags GetRenderingFlags()
    {
        return renderingFlags;
    }

    void SetRenderingFlags(Types::RenderingFlags flags)
    {
        renderingFlags = flags;
    }

    void Initialize()
    {
        renderingFlags = Types::RenderingFlags_DrawEverything;
    }
}  // namespace IWXMVM::Components::Rendering