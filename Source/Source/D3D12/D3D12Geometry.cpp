#include "D3D12Geometry.h"
#include "D3D12UploadBuffer.h"

void D3D12Geometry::UploadVertexBuffer()
{
	std::vector<Vertex> vertexList;
	//First Quad
	vertexList.push_back({ {-0.5f, 0.5f, 0.5f}, {1.0, 0.0, 1.0, 1.0} });
	vertexList.push_back({ {0.5f,  -0.5f,  0.5f}, {0.0, 1.0, 1.0, 1.0} });
	vertexList.push_back({ {-0.5f, -0.5f,  0.5f}, {1.0, 1.0, 0.0, 1.0} });
	vertexList.push_back({ {0.5f, 0.5f,  0.5f}, {1.0, 1.0, 1.0, 1.0} });
	
	//Second Quad
	vertexList.push_back({ {-0.75f, 0.75f, 0.75f}, {1.0, 1.0, 1.0, 1.0} });
	vertexList.push_back({ {0.0f,  0.0f,  0.75f}, {1.0, 1.0, 1.0, 1.0} });
	vertexList.push_back({ {-0.75f, 0.0f,  0.75f}, {1.0, 1.0, 1.0, 1.0} });
	vertexList.push_back({ {0.0f, 0.75f,  0.75f}, {1.0, 1.0, 1.0, 1.0} });

	uint32_t vertsDataSize = sizeof(Vertex) * vertexList.size();
	D3D12UploadBuffer::Upload(vertexBuffer, vertexList.data(), vertsDataSize);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = vertsDataSize;
	vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void D3D12Geometry::UploadIndexBuffer()
{
	std::vector<DWORD> indexList;
	indexList.push_back(0);
	indexList.push_back(1);
	indexList.push_back(2);
	indexList.push_back(0);
	indexList.push_back(3);
	indexList.push_back(1);

	uint32_t indexDataSize = sizeof(DWORD) * indexList.size();
	D3D12UploadBuffer::Upload(indexBuffer, indexList.data(), indexDataSize);

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexDataSize;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void D3D12Geometry::Upload()
{
	UploadIndexBuffer();
	UploadVertexBuffer();
}

void D3D12Geometry::Set(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Input Assembler
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	cmdList->IASetIndexBuffer(&indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

std::vector<D3D12_INPUT_ELEMENT_DESC> D3D12Geometry::GetVertexLayout()
{
	return
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

D3D12_RESOURCE_DESC D3D12Geometry::CreateVertexBufferDesc(uint32_t VertexCount)
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
