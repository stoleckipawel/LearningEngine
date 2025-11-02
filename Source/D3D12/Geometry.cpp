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
    // Plane (Quad) vertices
    vertexList.push_back({ 
		{ -0.5f,  0.5f, 0.0f }, 
		{ 0.0f, 0.0f},
		{ 1.0, 0.0, 0.0, 1.0 } 
		}); 


    vertexList.push_back({ 
		{  0.5f,  0.5f, 0.0f }, 
		{ 1.0f, 0.0f},
		{ 0.0, 1.0, 0.0, 1.0 } 
		}); 


    vertexList.push_back({ 
		{  0.5f, -0.5f, 0.0f }, 
		{ 1.0f, 1.0f},
		{ 0.0, 0.0, 1.0, 1.0 } 
		}); 


    vertexList.push_back({ 
		{ -0.5f, -0.5f, 0.0f }, 
		{ 0.0f, 1.0f},
		{ 1.0, 1.0, 0.0, 1.0 } 
		}); 

	const UINT vertsDataSize = sizeof(Vertex) * vertexList.size();
	vertexBuffer = UploadBuffer::Upload(vertexList.data(), vertsDataSize);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = vertsDataSize;
	vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void Geometry::UploadIndexBuffer()
{
	std::vector<DWORD> indexList;
    // Two triangles for the quad
    indexList.push_back(0); // Top-left
    indexList.push_back(1); // Top-right
    indexList.push_back(2); // Bottom-right

    indexList.push_back(0); // Top-left
    indexList.push_back(2); // Bottom-right
    indexList.push_back(3); // Bottom-left

	UINT indexDataSize = sizeof(DWORD) * indexList.size();
	indexBuffer = UploadBuffer::Upload(indexList.data(), indexDataSize);

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexDataSize;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Geometry::Upload()
{
	UploadIndexBuffer();
	UploadVertexBuffer();
}

void Geometry::Release()
{
	indexBuffer.Release();
	vertexBuffer.Release();
}

void Geometry::Set()
{
	//Input Assembler
	GRHI.GetCurrentCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	GRHI.GetCurrentCommandList()->IASetIndexBuffer(&indexBufferView);
	GRHI.GetCurrentCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
