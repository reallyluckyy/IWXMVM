#pragma once
#include "StdInclude.hpp"

#include "Graphics/Resource.hpp"
#include "Resources.hpp"
#include "Types/Keyframe.hpp"

namespace IWXMVM::GFX
{
    INCBIN_EXTERN(AXIS_MODEL);
    INCBIN_EXTERN(CAMERA_MODEL);
    INCBIN_EXTERN(ICOSPHERE_MODEL);
    INCBIN_EXTERN(GIZMO_TRANSLATE_MODEL);

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
              icosphere(ICOSPHERE_MODEL_data, ICOSPHERE_MODEL_size),
              gizmo_translate_x(GIZMO_TRANSLATE_MODEL_data, GIZMO_TRANSLATE_MODEL_size),
              gizmo_translate_y(GIZMO_TRANSLATE_MODEL_data, GIZMO_TRANSLATE_MODEL_size),
              gizmo_translate_z(GIZMO_TRANSLATE_MODEL_data, GIZMO_TRANSLATE_MODEL_size)
        {
        }

        void DrawTranslateGizmoArrow(Mesh& mesh, glm::mat4 model, int32_t axisIndex);
        bool MouseIntersects(ImVec2 mousePos, Mesh& mesh, glm::mat4 model);
        void DrawTranslationGizmo(glm::vec3& position, glm::mat4 translation, glm::mat4 rotation);

        void SetupRenderState() const noexcept;

        Mesh axis;
        Mesh camera;
        Mesh icosphere;
        Mesh gizmo_translate_x;
        Mesh gizmo_translate_y;
        Mesh gizmo_translate_z;
    };
}  // namespace IWXMVM