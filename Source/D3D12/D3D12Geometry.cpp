#include "D3D12Geometry.h"


void D3D12Geometry::UploadBuffer(ComPointer<ID3D12Resource2>& buffer, void* data, uint32_t dataSize)
{
	//Default Heap & Resource
	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Width = dataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12Context::Get().GetDevice()->CreateCommittedResource(
		&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&buffer));

	//Upload Heap & Resource
	D3D12_HEAP_PROPERTIES heapUploadProperties = {};
	heapUploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapUploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapUploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapUploadProperties.CreationNodeMask = 0;
	heapUploadProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC uploadResourceDesc{};
	uploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	uploadResourceDesc.Width = dataSize;
	uploadResourceDesc.Height = 1;
	uploadResourceDesc.DepthOrArraySize = 1;
	uploadResourceDesc.MipLevels = 1;
	uploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadResourceDesc.SampleDesc.Count = 1;
	uploadResourceDesc.SampleDesc.Quality = 0;
	uploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPointer<ID3D12Resource2> uploadBuffer;
	D3D12Context::Get().GetDevice()->CreateCommittedResource(
		&heapUploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	auto cmdList = D3D12Context::Get().InitializeCommandList();

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = reinterpret_cast<BYTE*>(data);
	subResourceData.RowPitch = dataSize;
	subResourceData.SlicePitch = dataSize;
	UpdateSubresources(cmdList, buffer, uploadBuffer, 0, 0, 1, &subResourceData);

	D3D12Context::Get().ExecuteCommandList();

	//uploadBuffer->Release();
}

void D3D12Geometry::Upload()
{
	Vertex verts[] =
	{
		// T1
		{{-1.0f, -1.0f, 0.0f}, {1.0, 1.0, 1.0, 1.0}},
		{{0.0f,  1.0f,  0.0f}, {1.0, 1.0, 1.0, 1.0}},
		{{1.0f, -1.0f,  0.0f}, {1.0, 1.0, 1.0, 1.0}}
	};

	uint32_t vertsDataSize = sizeof(Vertex) * _countof(verts);
	UploadBuffer(vertexBuffer, verts, vertsDataSize);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = vertsDataSize;
	vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void D3D12Geometry::Set(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Input Assembler
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

std::vector<D3D12_INPUT_ELEMENT_DESC> D3D12Geometry::GetVertexLayout()
{
	return
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
