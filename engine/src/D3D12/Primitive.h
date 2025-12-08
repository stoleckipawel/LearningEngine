#pragma once

#include "RHI.h"

 //Vertex structure for geometry
struct Vertex
{
	XMFLOAT3 position; // Vertex position (x, y, z)
	XMFLOAT2 uv;       // Texture coordinates (u, v)
	XMFLOAT4 color;    // Vertex color (r, g, b, a)
};

/**
 * @brief Primitive class for managing vertex and index buffers
 *
 * Handles uploading, releasing, and binding geometry resources for rendering.
 */
class Primitive
{
public:
    Primitive();

    // Sets geometry buffers and topology for rendering
    virtual void Set();

    // Returns the input layout for the vertex structure
    virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();

    // Creates a resource description for a vertex buffer
    virtual D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);

    // Uploads both vertex and index buffers
    void Upload();

protected:
    // These must be implemented by derived classes to provide mesh data
    virtual void GenerateVertices(std::vector<Vertex>& outVertices) const = 0;
    virtual void GenerateIndices(std::vector<DWORD>& outIndices) const = 0;

    // Uploads the vertex buffer for the geometry
    void UploadVertexBuffer();
    // Uploads the index buffer for the geometry
    void UploadIndexBuffer();

    ComPtr<ID3D12Resource2> VertexBuffer = nullptr;
    ComPtr<ID3D12Resource2> IndexBuffer = nullptr; 
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {}; 
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};  
};

