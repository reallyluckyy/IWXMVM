#pragma once
#include "StdInclude.hpp"

#include "Types/Vertex.hpp"

namespace IWXMVM::GFX
{
    inline constexpr std::size_t MAX_MESHES = 100;

    struct Mesh
    {
        Mesh(const uint8_t data[], uint32_t size);  // For use with incbin
        Mesh(){};

        std::vector<Types::Index> indices;
        std::vector<Types::Vertex> vertices;
        std::size_t index = 0; // Unique index returned by the buffer manager
        DWORD fillMode = D3DFILL_SOLID;
    };

    enum class BufferType
    {
        Static, Dynamic
    };

    class BufferManager
    {
       public:
        static BufferManager& Get()
        {
            static BufferManager instance;
            return instance;
        }

        BufferManager(BufferManager const&) = delete;
        void operator=(BufferManager const&) = delete;

        void Initialize();
        void Uninitialize();

        void AddMesh(Mesh* mesh, BufferType bufferType = BufferType::Static);
        void DrawMesh(const Mesh& mesh, const glm::mat4& model, bool ignoreLighting = false) const noexcept;
        void BindBuffers(BufferType bufferType) const noexcept;
        void ClearBuffers() noexcept;
        void ClearDynamicBuffers() noexcept;

       private:
        BufferManager()
        {
        }

        class IndexBuffer
        {
           public:
            void Initialize(bool map);
            void Release();
            void Add(std::span<Types::Index> elems);
            void Overwrite(std::span<Types::Index> elems);
            void Clear() noexcept;

            IDirect3DIndexBuffer9* GetHandle() const noexcept
            {
                return handle;
            }

            std::size_t GetElementCount() const noexcept
            {
                return elemCount;
            }

           private:
            IDirect3DIndexBuffer9* handle = nullptr;
            std::size_t elemCount = 0;
            std::optional<void*> data;  // has value while the buffer is mapped to cpu accessible memory
        };

        class VertexBuffer
        {
           public:
            void Initialize(bool map);
            void Release();
            void Add(std::span<Types::Vertex> elems);
            void Overwrite(std::span<Types::Vertex> elems);
            void Clear() noexcept;

            IDirect3DVertexBuffer9* GetHandle() const noexcept
            {
                return handle;
            }

            std::size_t GetElementCount() const noexcept
            {
                return elemCount;
            }

           private:
            IDirect3DVertexBuffer9* handle = nullptr;
            std::size_t elemCount = 0;
            std::optional<void*> data;  // has value while the buffer is mapped to cpu accessible memory
        };

        struct MeshMetadata
        {
            Mesh* ptr = nullptr;
            std::size_t indexBufferOffset = 0;
            std::size_t vertexBufferOffset = 0;
        };

        IndexBuffer staticIndexBuffer;
        VertexBuffer staticVertexBuffer;
        IndexBuffer dynamicIndexBuffer;
        VertexBuffer dynamicVertexBuffer;
        std::array<MeshMetadata, MAX_MESHES> meshes;
        std::size_t meshCount = 0;
    };
}  // namespace IWXMVM::GFX