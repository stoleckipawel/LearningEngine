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
 * @brief Geometry class for managing vertex and index buffers
 *
 * Handles uploading, releasing, and binding geometry resources for rendering.
 */
class Geometry
{
public:
	// Constructs and uploads geometry resources
	Geometry();

	// Destructor releases geometry resources
	~Geometry();

	// Deleted copy constructor and assignment operator to enforce unique ownership
	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;

	// Sets geometry buffers and topology for rendering
	void Set();

	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();

	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);
private:
	// Uploads the vertex buffer for the geometry
	void UploadVertexBuffer();
	// Uploads the index buffer for the geometry
	void UploadIndexBuffer();
	// Uploads both vertex and index buffers
	void Upload();
	// Releases geometry resources
	void Release();
private:
	ComPointer<ID3D12Resource2> VertexBuffer = nullptr;
	ComPointer<ID3D12Resource2> IndexBuffer = nullptr; 
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {}; 
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};  
};

