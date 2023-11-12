#pragma once
#include "StdInclude.hpp"

#include "Types/Vertex.hpp"

namespace IWXMVM::GFX
{
    inline constexpr std::size_t MAX_MESHES = 100;

    struct Mesh
    {
        Mesh(const uint8_t data[], uint32_t size);  // For use with incbin

        std::vector<Types::Index> indices;
        std::vector<Types::Vertex> vertices;
        std::size_t index = 0; // Unique index returned by the buffer manager
        DWORD fillMode = D3DFILL_SOLID;
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

        void AddMesh(Mesh* mesh);
        void DrawMesh(const Mesh& mesh, const glm::mat4& model) const noexcept;
        void BindBuffers() const noexcept;
        void ClearBuffers() noexcept;

        IDirect3DVertexBuffer9* GetVertexBufferHandle() const noexcept
        {
            return vertexBuffer.GetHandle();
        }
        IDirect3DIndexBuffer9* GetIndexBufferHandle() const noexcept
        {
            return indexBuffer.GetHandle();
        }
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

        IndexBuffer indexBuffer;
        VertexBuffer vertexBuffer;
        std::array<MeshMetadata, MAX_MESHES> meshes;
        std::size_t meshCount = 0;
    };
}  // namespace IWXMVM::GFX