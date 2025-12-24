#pragma once

#include "D3D12Rhi.h"
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// Vertex structure for geometry: position, UV, and color.
struct Vertex
{
    XMFLOAT3 position; // Vertex position (x, y, z)
    XMFLOAT2 uv;       // Texture coordinates (u, v)
    XMFLOAT4 color;    // Vertex color (r, g, b, a)
};

// Base class for renderable primitives. Handles upload/binding and per-frame resources.
class Primitive
{
public:
    // Construct a new Primitive. translation/rotation/scale default to identity (no transform).
    Primitive(
        const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

    // Virtual destructor for polymorphic base
    virtual ~Primitive() = default;

    // Transform API:  explicit setters/getters
    void SetTranslation(const XMFLOAT3& t) noexcept;
    XMFLOAT3 GetTranslation() const noexcept;

    void SetRotationEuler(const XMFLOAT3& r) noexcept; // Euler angles (radians)
    XMFLOAT3 GetRotationEuler() const noexcept;

    void SetScale(const XMFLOAT3& s) noexcept;
    XMFLOAT3 GetScale() const noexcept;

    // Compute the world transformation matrix from TRS and return it. This is cached
    // internally and lazily rebuilt on transform changes to avoid repeated work.
    XMMATRIX GetWorldMatrix() const noexcept;

    // Return the 3x3 rotation-only matrix (useful for normal transforms on CPU).
    XMFLOAT3X3 GetWorldRotationMatrix3x3() const noexcept;

    // Return inverse-transpose of world for correct normal transformation in shaders.
    XMMATRIX GetWorldInverseTransposeMatrix() const noexcept;

    // Return the number of indices in the index buffer.
    UINT GetIndexCount() const noexcept { return m_indexCount; }

    // Set geometry buffers and topology for rendering. Override to bind more resources.
    virtual void Set();

    // Return the input layout for the vertex structure. Override to customize.
    virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();

    // Create a D3D12 resource description for a vertex buffer with VertexCount vertices.
    virtual D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);

    // Upload both vertex and index buffers to the GPU.
    void Upload();

protected:
    // Must be implemented by derived classes to populate mesh vertex data into outVertices.
    virtual void GenerateVertices(std::vector<Vertex>& outVertices) const = 0;

    // Must be implemented by derived classes to populate mesh index data into outIndices.
    virtual void GenerateIndices(std::vector<DWORD>& outIndices) const = 0;

    // Upload the vertex buffer for the geometry.
    void UploadVertexBuffer();

    // Upload the index buffer for the geometry.
    void UploadIndexBuffer();

    // Internal: mark cached world invalid (called from setters)
    void InvalidateWorldCache() noexcept { m_worldDirty = true; }

private:
    // Transform state (encapsulated)
    XMFLOAT3 m_translation{0.0f, 0.0f, 0.0f};
    XMFLOAT3 m_rotationEuler{0.0f, 0.0f, 0.0f};
    XMFLOAT3 m_scale{1.0f, 1.0f, 1.0f};

    // Cached world matrix to avoid recomputing each time. Mutable so const accessors
    // can rebuild lazily without exposing mutability to callers.
    mutable XMFLOAT4X4 m_worldMatrixCache{};
    mutable bool m_worldDirty = true;

    // Lazy rebuild helper: rebuild the cached world matrix when dirty.
    void RebuildWorldIfNeeded() const noexcept;

protected:
    // GPU resources and views
    ComPtr<ID3D12Resource2> VertexBuffer = nullptr; ///< Vertex buffer resource
    ComPtr<ID3D12Resource2> IndexBuffer = nullptr;  ///< Index buffer resource
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {}; ///< Vertex buffer view
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};   ///< Index buffer view
    UINT m_indexCount = 0; ///< Number of indices in the index buffer
};

