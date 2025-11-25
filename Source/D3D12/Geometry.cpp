#include "Geometry.h"
#include "UploadBuffer.h"

Geometry::~Geometry()
{
	Release();
}

void Geometry::UploadVertexBuffer()
{
	//Position
	//UV
	//Color

	std::vector<Vertex> vertexList;
	// Cube vertices
	vertexList = {
		// Front face
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0, 0.0, 0.0, 1.0 } }, // 0
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0, 1.0, 0.0, 1.0 } }, // 1
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 0.0, 0.0, 1.0, 1.0 } }, // 2
		{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 1.0, 1.0, 0.0, 1.0 } }, // 3

		// Back face
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0, 0.0, 1.0, 1.0 } }, // 4
		{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0, 1.0, 1.0, 1.0 } }, // 5
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 1.0, 1.0, 1.0, 1.0 } }, // 6
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.5, 0.5, 0.5, 1.0 } }, // 7
	};

	const UINT vertsDataSize = sizeof(Vertex) * vertexList.size();
	VertexBuffer = UploadBuffer::Upload(vertexList.data(), vertsDataSize);

	m_vertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = vertsDataSize;
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void Geometry::UploadIndexBuffer()
{
	std::vector<DWORD> indexList;
	// Cube indices (12 triangles, 36 indices)
	indexList = {
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

	UINT indexDataSize = sizeof(DWORD) * indexList.size();
	IndexBuffer = UploadBuffer::Upload(indexList.data(), indexDataSize);

	m_indexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = indexDataSize;
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Geometry::Upload()
{
	UploadIndexBuffer();
	UploadVertexBuffer();
}

void Geometry::Release()
{
	IndexBuffer.Release();
	VertexBuffer.Release();
}

void Geometry::Set()
{
	//Input Assembler
	GRHI.GetCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	GRHI.GetCommandList()->IASetIndexBuffer(&m_indexBufferView);
	GRHI.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

std::vector<D3D12_INPUT_ELEMENT_DESC> Geometry::GetVertexLayout()
{
	return
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

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
