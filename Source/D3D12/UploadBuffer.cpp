#include "UploadBuffer.h"

ComPointer<ID3D12Resource2> FUploadBuffer::Upload(void* data, uint32_t dataSize)
{
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

	//Upload Heap
	D3D12_HEAP_PROPERTIES heapUploadProperties = {};
	heapUploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	ComPointer<ID3D12Resource2> uploadBuffer;
	ThrowIfFailed(GRHI.Device->CreateCommittedResource(
		&heapUploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer)),
		"UploadBuffer: Failed To Create Committed Resource");

    // Map upload buffer and copy data
    void* mappedData = nullptr;
    D3D12_RANGE readRange = { 0, 0 };
    ThrowIfFailed(uploadBuffer->Map(0, &readRange, &mappedData), "UploadBuffer: Failed To Map Upload Buffer");
    memcpy(mappedData, data, dataSize);
    uploadBuffer->Unmap(0, nullptr);

	//To Do: Use Default Heap & another Commited Resource for better performance
	return uploadBuffer;
}