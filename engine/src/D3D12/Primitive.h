#pragma once

#include "RHI.h"
#include "ConstantBuffer.h"


//------------------------------------------------------------------------------
// Constant buffer and vertex data structures
//------------------------------------------------------------------------------

/**
 * @brief Per-pixel constant buffer data (aligned to 256 bytes for D3D12)
 *
 * Used to pass color and other per-pixel data to the pixel shader.
 */
struct alignas(256) PixelConstantBufferData
{
    XMFLOAT4 Color;           // RGBA color
};

/**
 * @brief Per-vertex constant buffer data (aligned to 256 bytes for D3D12)
 *
 * Used to pass transformation matrices to the vertex shader.
 */
struct alignas(256) FVertexConstantBufferData
{
    XMFLOAT4X4 WorldMTX;      // World transformation matrix
    XMFLOAT4X4 ViewMTX;       // View transformation matrix
    XMFLOAT4X4 ProjectionMTX; // Projection transformation matrix
    XMFLOAT4X4 WorldViewProjMTX; // Combined World-View-Projection matrix
};

/**
 * @brief Vertex structure for geometry
 *
 * Contains position, texture coordinates, and color for each vertex.
 */
struct Vertex
{
    XMFLOAT3 position; // Vertex position (x, y, z)
    XMFLOAT2 uv;       // Texture coordinates (u, v)
    XMFLOAT4 color;    // Vertex color (r, g, b, a)
};


/**
 * @brief Base class for all renderable primitives (geometry objects).
 *
 * Handles uploading, releasing, and binding geometry resources for rendering.
 * Extend this class to implement custom shapes by providing vertex/index data.
 */
class Primitive
{
public:
    /**
     * @brief Construct a new Primitive object
     * @param translation Initial translation (position) in world space
     * @param rotation Initial rotation (Euler angles, radians)
     * @param scale Initial scale
     *
     * All parameters default to identity (no translation, no rotation, unit scale).
     */
    Primitive(
        const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

    // Transformation inputs
    XMFLOAT3 Translation = {0.0f, 0.0f, 0.0f}; ///< World position
    XMFLOAT3 Rotation = {0.0f, 0.0f, 0.0f};    ///< Euler angles in radians
    XMFLOAT3 Scale = {1.0f, 1.0f, 1.0f};       ///< Local scale

    /**
     * @brief Computes the world transformation matrix from TRS.
     * @return XMMATRIX The world matrix.
     */
    XMMATRIX GetWorldMatrix() const;

    /**
     * @brief Updates all constant buffers for this primitive (vertex & pixel).
     */
    void UpdateConstantBuffers();

    /**
     * @brief Get the vertex constant buffer for the current frame.
     */
    ConstantBuffer<FVertexConstantBufferData>* GetVertexConstantBuffer() { return VertexConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }

    /**
     * @brief Get the pixel constant buffer for the current frame.
     */
    ConstantBuffer<PixelConstantBufferData>* GetPixelConstantBuffer() { return PixelConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }

    /**
     * @brief Get the number of indices in the index buffer.
     */
    UINT GetIndexCount() const { return m_indexCount; }

    /**
     * @brief Sets geometry buffers and topology for rendering.
     * Override to bind additional resources if needed.
     */
    virtual void Set();

    /**
     * @brief Returns the input layout for the vertex structure.
     * Override to provide custom layouts.
     */
    virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();

    /**
     * @brief Creates a resource description for a vertex buffer.
     * @param VertexCount Number of vertices.
     * @return D3D12_RESOURCE_DESC Resource description.
     */
    virtual D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);

    /**
     * @brief Uploads both vertex and index buffers to the GPU.
     */
    void Upload();

protected:
    /**
     * @brief Must be implemented by derived classes to provide mesh vertex data.
     * @param outVertices Output vector to be filled with vertex data.
     */
    virtual void GenerateVertices(std::vector<Vertex>& outVertices) const = 0;

    /**
     * @brief Must be implemented by derived classes to provide mesh index data.
     * @param outIndices Output vector to be filled with index data.
     */
    virtual void GenerateIndices(std::vector<DWORD>& outIndices) const = 0;

    /**
     * @brief Uploads the vertex buffer for the geometry.
     */
    void UploadVertexBuffer();

    /**
     * @brief Uploads the index buffer for the geometry.
     */
    void UploadIndexBuffer();

    /**
     * @brief Updates the vertex constant buffer for the current frame.
     */
    void UpdateVertexConstantBuffer();

    /**
     * @brief Updates the pixel constant buffer for the current frame.
     */
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

