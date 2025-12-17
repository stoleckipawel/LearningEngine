#pragma once

#include "RHI.h"
#include "ConstantBuffer.h"


//------------------------------------------------------------------------------
// Constant buffer and vertex data structures
//------------------------------------------------------------------------------

// Per-pixel constant buffer data (aligned to 256 bytes). Used by the pixel shader.
struct alignas(256) PixelConstantBufferData
{
    XMFLOAT4 Color;           // RGBA color
};

// Per-vertex constant buffer data (aligned to 256 bytes). Holds matrices for the vertex shader.
struct alignas(256) FVertexConstantBufferData
{
    XMFLOAT4X4 WorldMTX;      // World transformation matrix
    XMFLOAT4X4 ViewMTX;       // View transformation matrix
    XMFLOAT4X4 ProjectionMTX; // Projection transformation matrix
    XMFLOAT4X4 WorldViewProjMTX; // Combined World-View-Projection matrix
};

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

    // Transformation inputs
    XMFLOAT3 Translation = {0.0f, 0.0f, 0.0f}; ///< World position
    XMFLOAT3 Rotation = {0.0f, 0.0f, 0.0f};    ///< Euler angles in radians
    XMFLOAT3 Scale = {1.0f, 1.0f, 1.0f};       ///< Local scale

    // Compute the world transformation matrix from TRS and return it.
    XMMATRIX GetWorldMatrix() const;

    // Update all constant buffers for this primitive (vertex & pixel).
    void UpdateConstantBuffers();

    // Get the vertex constant buffer for the current frame.
    ConstantBuffer<FVertexConstantBufferData>* GetVertexConstantBuffer() { return VertexConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }

    // Get the pixel constant buffer for the current frame.
    ConstantBuffer<PixelConstantBufferData>* GetPixelConstantBuffer() { return PixelConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }

    // Return the number of indices in the index buffer.
    UINT GetIndexCount() const { return m_indexCount; }

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

    // Update the vertex constant buffer for the current frame.
    void UpdateVertexConstantBuffer();

    // Update the pixel constant buffer for the current frame.
    void UpdatePixelConstantBuffer();

    // GPU resources and views
    ComPtr<ID3D12Resource2> VertexBuffer = nullptr; ///< Vertex buffer resource
    ComPtr<ID3D12Resource2> IndexBuffer = nullptr;  ///< Index buffer resource
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {}; ///< Vertex buffer view
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};   ///< Index buffer view
    UINT m_indexCount = 0; ///< Number of indices in the index buffer

    // Per-frame constant buffers
    std::unique_ptr<ConstantBuffer<FVertexConstantBufferData>> VertexConstantBuffer[EngineSettings::FramesInFlight];
    std::unique_ptr<ConstantBuffer<PixelConstantBufferData>> PixelConstantBuffer[EngineSettings::FramesInFlight];
};

