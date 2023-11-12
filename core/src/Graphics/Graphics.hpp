#pragma once
#include "StdInclude.hpp"

#include "Graphics/Resource.hpp"
#include "Resources.hpp"

namespace IWXMVM::GFX
{
    INCBIN_EXTERN(AXIS_MODEL);
    INCBIN_EXTERN(CAMERA_MODEL);
    INCBIN_EXTERN(ICOSPHERE_MODEL);

    class GraphicsManager
    {
       public:
        static GraphicsManager& Get()
        {
            static GraphicsManager instance;
            return instance;
        }

        GraphicsManager(GraphicsManager const&) = delete;
        void operator=(GraphicsManager const&) = delete;

        void Initialize();
        void Uninitialize();
        void Render();
       private:
        GraphicsManager()
            : axis(AXIS_MODEL_data, AXIS_MODEL_size),
              camera(CAMERA_MODEL_data, CAMERA_MODEL_size),
              icosphere(ICOSPHERE_MODEL_data, ICOSPHERE_MODEL_size)
        {
        }

        void SetupRenderState() const noexcept;

        Mesh axis;
        Mesh camera;
        Mesh icosphere;
    };
}  // namespace IWXMVM