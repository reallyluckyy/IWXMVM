#include "StdInclude.hpp"

#include "Graphics/Resource.hpp"

#include "D3D9.hpp"
#include "Types/Vertex.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace std
{
    template <>
    struct hash<IWXMVM::Types::Vertex>
    {
        size_t operator()(IWXMVM::Types::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                   (hash<D3DCOLOR>()(vertex.col) << 1);
        }
    };
}  // namespace std

namespace IWXMVM::GFX
{
    constexpr std::size_t MAX_VERTICES = 50000;
    constexpr std::size_t MAX_INDICES = MAX_VERTICES * 5;

    Mesh::Mesh(const uint8_t data[], uint32_t size)
    {
        std::string_view dataStringView(reinterpret_cast<const char*>(data), size);
        std::string dataString(dataStringView);

        tinyobj::ObjReader reader;

        if (!reader.ParseFromString(dataString, ""))
        {
            if (!reader.Error().empty())
            {
                LOG_ERROR("TinyObjReader: {}", reader.Error());
            }
            return;
        }

        if (!reader.Warning().empty())
        {
            LOG_WARN("TinyObjReader: {}", reader.Warning());
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();

        vertices.reserve(attrib.vertices.size() / 3);

        // Loop over shapes
        std::unordered_map<Types::Vertex, std::uint32_t> uniqueVertices;
        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Types::Vertex vertex = {};
                vertex.pos = {
                    attrib.vertices[index.vertex_index * 3 + 0],
                    attrib.vertices[index.vertex_index * 3 + 1],
                    attrib.vertices[index.vertex_index * 3 + 2],
                };
                vertex.normal = {
                    attrib.normals[index.normal_index * 3 + 0],
                    attrib.normals[index.normal_index * 3 + 1],
                    attrib.normals[index.normal_index * 3 + 2],
                };
                vertex.col = D3DCOLOR_COLORVALUE(attrib.colors[index.vertex_index * 3 + 0],
                                                 attrib.colors[index.vertex_index * 3 + 1],
                                                 attrib.colors[index.vertex_index * 3 + 2], 1.0f);
                
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<std::uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void BufferManager::Initialize()
    {
        indexBuffer.Initialize(false);
        vertexBuffer.Initialize(false);
    }

    void BufferManager::Uninitialize()
    {
        indexBuffer.Release();
        vertexBuffer.Release();
        meshCount = 0;
    }

    void BufferManager::AddMesh(Mesh* mesh)
    {
        MeshMetadata metadata = {.ptr = mesh,
                                 .indexBufferOffset = indexBuffer.GetElementCount(),
                                 .vertexBufferOffset = vertexBuffer.GetElementCount()};
        indexBuffer.Add(mesh->indices);
        vertexBuffer.Add(mesh->vertices);
        meshes[meshCount++] = metadata;

        // meshCount is always strictly greater than 0 here so this is safe
        mesh->index = meshCount - 1;
    }

    void BufferManager::DrawMesh(const Mesh& mesh, const glm::mat4& model, bool ignoreLighting) const noexcept
    {
        IDirect3DDevice9* device = D3D9::GetDevice();

        HRESULT result = D3D_OK;

        device->SetVertexShaderConstantF(4, reinterpret_cast<const float*>(&model), 4);
        device->SetPixelShaderConstantB(0, reinterpret_cast<BOOL*>(&ignoreLighting), 1);

        result = device->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST, meshes[mesh.index].vertexBufferOffset, 0, static_cast<UINT>(mesh.indices.size()),
            meshes[mesh.index].indexBufferOffset, static_cast<UINT>(mesh.indices.size() / 3));

        if (FAILED(result))
        {
            LOG_WARN("Failed to issue draw mesh with index {}", mesh.index);
        }
    }

    void BufferManager::BindBuffers() const noexcept
    {
        IDirect3DDevice9* device = D3D9::GetDevice();

        HRESULT result = device->SetStreamSource(0, vertexBuffer.GetHandle(), 0, sizeof(Types::Vertex));
        if (FAILED(result))
        {
            LOG_WARN("Failed to bind vertex buffer");
        }

        result = device->SetIndices(indexBuffer.GetHandle());
        if (FAILED(result))
        {
            LOG_WARN("Failed to bind index buffer");
        }
    }

    void BufferManager::ClearBuffers() noexcept
    {
        vertexBuffer.Clear();
        indexBuffer.Clear();
        meshCount = 0;
    }

    void BufferManager::IndexBuffer::Initialize(bool map)
    {
        IDirect3DDevice9* device = D3D9::GetDevice();
        constexpr std::size_t byteSize = MAX_INDICES * sizeof(Types::Index);

        HRESULT result = device->CreateIndexBuffer(static_cast<UINT>(byteSize), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32,
                                           D3DPOOL_DEFAULT, &handle, nullptr);
        if (FAILED(result))
        {
            throw std::runtime_error("Failed to create index buffer");
        }

        if (map)
        {
            void* tmp = nullptr;
            result = handle->Lock(0, static_cast<UINT>(byteSize), &tmp, 0);
            if (FAILED(result))
            {
                throw std::runtime_error("Failed to lock index buffer");
            }

            data = tmp;
        }
    }

    void BufferManager::IndexBuffer::Release()
    {
        if (data.has_value())
        {
            data.reset();
            handle->Unlock();
        }

        handle->Release();
    }

    void BufferManager::IndexBuffer::Add(std::span<Types::Index> elems)
    {
        if (elems.empty())
        {
            return;
        }

        const std::size_t totalBufferByteSize = MAX_INDICES * sizeof(elems[0]);
        const std::size_t bufferByteSize = elemCount * sizeof(elems[0]);
        const std::size_t elemsByteSize = elems.size() * sizeof(elems[0]);

        if (bufferByteSize + elemsByteSize > totalBufferByteSize)
        {
            LOG_WARN("D3D9 index buffer will overflow, ignoring 'Add' call");
            return;
        }

        const bool mapped = data.has_value();
        if (!mapped)
        {
            void* tmp = nullptr;
            HRESULT result = handle->Lock(bufferByteSize, static_cast<UINT>(elemsByteSize), &tmp, 0);
            if (FAILED(result))
            {
                throw std::runtime_error("Failed to lock index buffer");
            }

            std::memcpy(tmp, elems.data(), static_cast<UINT>(elemsByteSize));

            handle->Unlock();
        }
        else
        {
            std::memcpy(reinterpret_cast<std::uint8_t*>(data.value()) + bufferByteSize, elems.data(),
                        static_cast<UINT>(elemsByteSize));
        }

        elemCount += elems.size();
    }

    void BufferManager::IndexBuffer::Overwrite(std::span<Types::Index> elems)
    {
        if (elems.empty())
        {
            return;
        }

        const std::size_t totalBufferByteSize = MAX_INDICES * sizeof(elems[0]);
        const std::size_t elemsByteSize = elems.size() * sizeof(elems[0]);

        if (elemsByteSize > totalBufferByteSize)
        {
            LOG_WARN("D3D9 index buffer will overflow, ignoring 'Overwrite' call");
            return;
        }

        const bool mapped = data.has_value();
        if (!mapped)
        {
            void* tmp = nullptr;
            HRESULT result = handle->Lock(0, static_cast<UINT>(elemsByteSize), &tmp, 0);
            if (FAILED(result))
            {
                throw std::runtime_error("Failed to lock index buffer");
            }

            data = tmp;
        }

        std::memcpy(data.value(), elems.data(), static_cast<UINT>(elemsByteSize));

        if (!mapped)
        {
            data.reset();
            handle->Unlock();
        }

        elemCount += elems.size();
    }

    void BufferManager::IndexBuffer::Clear() noexcept
    {
        elemCount = 0;
    }

    void BufferManager::VertexBuffer::Initialize(bool map)
    {
        IDirect3DDevice9* device = D3D9::GetDevice();
        constexpr std::size_t byteSize = MAX_VERTICES * sizeof(Types::Vertex);

        HRESULT result = D3D_OK;
        result = device->CreateVertexBuffer(static_cast<UINT>(byteSize), D3DUSAGE_WRITEONLY,
                                            D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &handle, nullptr);

        if (FAILED(result))
        {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        if (map)
        {
            void* tmp = nullptr;
            result = handle->Lock(0, static_cast<UINT>(byteSize), &tmp, 0);
            if (FAILED(result))
            {
                throw std::runtime_error("Failed to lock vertex buffer");
            }

            data = tmp;
        }
    }

    void BufferManager::VertexBuffer::Release()
    {
        if (data.has_value())
        {
            data.reset();
            handle->Unlock();
        }

        handle->Release();
    }

    void BufferManager::VertexBuffer::Add(std::span<Types::Vertex> elems)
    {
        if (elems.empty())
        {
            return;
        }

        constexpr std::size_t totalBufferByteSize = MAX_VERTICES * sizeof(elems[0]);
        const std::size_t bufferByteSize = elemCount * sizeof(elems[0]);
        const std::size_t elemsByteSize = elems.size() * sizeof(elems[0]);

        if (bufferByteSize + elemsByteSize > totalBufferByteSize)
        {
            LOG_WARN("D3D9 vertex buffer will overflow, ignoring 'Add' call");
            return;
        }

        const bool mapped = data.has_value();
        if (!mapped)
        {
            void* tmp = nullptr;
            HRESULT result = handle->Lock(bufferByteSize, static_cast<UINT>(elemsByteSize), &tmp, 0);
            if (FAILED(result))
            {
                throw std::runtime_error("Failed to lock index buffer");
            }

            std::memcpy(tmp, elems.data(), static_cast<UINT>(elemsByteSize));

            handle->Unlock();
        }
        else
        {
            std::memcpy(reinterpret_cast<std::uint8_t*>(data.value()) + bufferByteSize, elems.data(),
                        static_cast<UINT>(elemsByteSize));
        }

        elemCount += elems.size();
    }

    void BufferManager::VertexBuffer::Overwrite(std::span<Types::Vertex> elems)
    {
        if (elems.empty())
        {
            return;
        }

        constexpr std::size_t totalBufferByteSize = MAX_VERTICES * sizeof(elems[0]);
        const std::size_t elemsByteSize = elems.size() * sizeof(elems[0]);

        if (elemsByteSize > totalBufferByteSize)
        {
            LOG_WARN("D3D9 vertex buffer will overflow, ignoring 'Overwrite' call");
            return;
        }

        const bool mapped = data.has_value();
        if (!mapped)
        {
            void* tmp = nullptr;
            HRESULT result = handle->Lock(0, static_cast<UINT>(elemsByteSize), &tmp, 0);
            if (FAILED(result))
            {
                throw std::runtime_error("Failed to lock vertex buffer");
            }

            data = tmp;
        }

        std::memcpy(data.value(), elems.data(), static_cast<UINT>(elemsByteSize));

        if (!mapped)
        {
            data.reset();
            handle->Unlock();
        }

        elemCount = elems.size();
    }

    void BufferManager::VertexBuffer::Clear() noexcept
    {
        elemCount = 0;
    }
}  // namespace IWXMVM::GFX