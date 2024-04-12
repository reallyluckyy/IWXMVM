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
    INCBIN_EXTERN(GIZMO_ROTATE_MODEL);

    enum GizmoMode
    {
        TranslateLocal,
        TranslateGlobal,
        Rotate,
        Count
    };

    struct TranslationGizmoData
    {
        std::int32_t axisIndex{};
        glm::vec3 objectOffset{};
    };

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
        void DrawDepth() const;

        std::optional<int32_t> GetSelectedNodeId() const { return selectedNodeId; }
        bool WasObjectHoveredThisFrame() const { return objectHoveredThisFrame; }
        bool WasObjectDraggedThisFrame() const { return heldAxis.has_value(); }
        
        void ResetSelection()
        {
            selectedNodeId = std::nullopt;
            heldAxis = std::nullopt;
        }

        GizmoMode GetGizmoMode() const { return gizmoMode; }
        void SetGizmoMode(GizmoMode mode) { gizmoMode = mode; }
       
       private:
        GraphicsManager()
            : axis(AXIS_MODEL_data, AXIS_MODEL_size),
              camera(CAMERA_MODEL_data, CAMERA_MODEL_size),
              icosphere(ICOSPHERE_MODEL_data, ICOSPHERE_MODEL_size),
              gizmo_translate_x(GIZMO_TRANSLATE_MODEL_data, GIZMO_TRANSLATE_MODEL_size),
              gizmo_translate_y(GIZMO_TRANSLATE_MODEL_data, GIZMO_TRANSLATE_MODEL_size),
              gizmo_translate_z(GIZMO_TRANSLATE_MODEL_data, GIZMO_TRANSLATE_MODEL_size),
              gizmo_rotate_x(GIZMO_ROTATE_MODEL_data, GIZMO_ROTATE_MODEL_size),
              gizmo_rotate_y(GIZMO_ROTATE_MODEL_data, GIZMO_ROTATE_MODEL_size),
              gizmo_rotate_z(GIZMO_ROTATE_MODEL_data, GIZMO_ROTATE_MODEL_size),
              campath()
        {
        }

        bool MouseIntersects(ImVec2 mousePos, Mesh& mesh, glm::mat4 model);
        bool MouseIntersectsSphereAt(ImVec2 mousePos, glm::vec3 pos, float radius);
        void DrawGizmoComponent(Mesh& mesh, glm::mat4 model, int32_t axisIndex);
        void DrawTranslationGizmo(glm::vec3& position, glm::mat4 translation, glm::mat4 rotation);
        void DrawRotationGizmo(glm::vec3& rotation, glm::mat4 translation);

        
        void BuildCampathMesh();
        void SetupRenderState() const noexcept;

        void CreateGraphicsResources();
        void DestroyGraphicsResources();
        IDirect3DPixelShader9* pixelShader = nullptr;
        IDirect3DVertexShader9* vertexShader = nullptr;
        IDirect3DVertexDeclaration9* vertexDeclaration = nullptr;

        void CreateDepthPassResources();
        void DestroyDepthPassResources();
        IDirect3DPixelShader9* depthPassPS = nullptr;
        IDirect3DVertexShader9* depthPassVS = nullptr;
        IDirect3DVertexDeclaration9* depthPassVDecl = nullptr;
        IDirect3DVertexBuffer9* depthPassVertices = nullptr;

        Mesh axis;
        Mesh camera;
        Mesh icosphere;
        Mesh gizmo_translate_x;
        Mesh gizmo_translate_y;
        Mesh gizmo_translate_z;
        Mesh gizmo_rotate_x;
        Mesh gizmo_rotate_y;
        Mesh gizmo_rotate_z;
        Mesh campath;

        std::optional<int32_t> selectedNodeId = std::nullopt;
        std::optional<TranslationGizmoData> heldAxis = std::nullopt;
        bool objectHoveredThisFrame = false;
        GizmoMode gizmoMode = GizmoMode::TranslateLocal;
    };
}  // namespace IWXMVM
