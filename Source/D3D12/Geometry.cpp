
#include "Geometry.h"
#include "UploadBuffer.h"

// Geometry.cpp
// Implements geometry setup, upload, and binding for rendering.


// Destructor: Releases geometry resources
Geometry::~Geometry()
{
	Release();
}

void Geometry::UploadVertexBuffer()
{
	// Define cube vertices with position, UV, and color
	std::vector<Vertex> vertexList = {
		// Front face
		{ { -0.25f, -0.25f,  0.25f }, { 0.0f, 1.0f }, { 1.0, 0.0, 0.0, 1.0 } }, // 0
		{ { -0.25f,  0.25f,  0.25f }, { 0.0f, 0.0f }, { 0.0, 1.0, 0.0, 1.0 } }, // 1
		{ {  0.25f,  0.25f,  0.25f }, { 1.0f, 0.0f }, { 0.0, 0.0, 1.0, 1.0 } }, // 2
		{ {  0.25f, -0.25f,  0.25f }, { 1.0f, 1.0f }, { 1.0, 1.0, 0.0, 1.0 } }, // 3

		// Back face
		{ { -0.25f, -0.25f, -0.25f }, { 1.0f, 1.0f }, { 1.0, 0.0, 1.0, 1.0 } }, // 4
		{ { -0.25f,  0.25f, -0.25f }, { 1.0f, 0.0f }, { 0.0, 1.0, 1.0, 1.0 } }, // 5
		{ {  0.25f,  0.25f, -0.25f }, { 0.0f, 0.0f }, { 1.0, 1.0, 1.0, 1.0 } }, // 6
		{ {  0.25f, -0.25f, -0.25f }, { 0.0f, 1.0f }, { 0.5, 0.5, 0.5, 1.0 } }, // 7
	};

	const UINT vertsDataSize = static_cast<UINT>(sizeof(Vertex) * vertexList.size());
	VertexBuffer = UploadBuffer::Upload(vertexList.data(), vertsDataSize);

	// Setup vertex buffer view
	m_vertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = vertsDataSize;
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}


// Uploads the index buffer
void Geometry::UploadIndexBuffer()
{
	// Define cube indices (12 triangles, 36 indices)
	std::vector<DWORD> indexList = {
		// Front face
		0, 1, 2, 0, 2, 3,
		// Back face
		4, 6, 5, 4, 7, 6,
		// Left face
		4, 5, 1, 4, 1, 0,
		// Right face
		3, 2, 6, 3, 6, 7,
		// Top face
		1, 5, 6, 1, 6, 2,
		// Bottom face
		4, 0, 3, 4, 3, 7
	};

	UINT indexDataSize = static_cast<UINT>(sizeof(DWORD) * indexList.size());
	IndexBuffer = UploadBuffer::Upload(indexList.data(), indexDataSize);

	// Setup index buffer view
	m_indexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = indexDataSize;
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}


// Uploads both vertex and index buffers
void Geometry::Upload()
{
	UploadIndexBuffer();
	UploadVertexBuffer();
}


// Releases geometry resources
void Geometry::Release()
{
	IndexBuffer.Release();
	VertexBuffer.Release();
}


// Sets geometry buffers and topology for rendering
void Geometry::Set()
{
	// Bind vertex buffer
	GRHI.GetCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	// Bind index buffer
	GRHI.GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
	// Set primitive topology
	GRHI.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


// Returns the input layout for the vertex structure
std::vector<D3D12_INPUT_ELEMENT_DESC> Geometry::GetVertexLayout()
{
	return {
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}


// Creates a resource description for a vertex buffer
D3D12_RESOURCE_DESC Geometry::CreateVertexBufferDesc(uint32_t VertexCount)
{
	D3D12_RESOURCE_DESC vertexResourceDesc = {};
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	vertexResourceDesc.Width = VertexCount;
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexResourceDesc.SampleDesc.Count = 1;
	vertexResourceDesc.SampleDesc.Quality = 0;
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	return vertexResourceDesc;
}
