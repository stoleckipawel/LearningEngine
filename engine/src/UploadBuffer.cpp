#include "PCH.h"
#include "UploadBuffer.h"
#include "DebugUtils.h"

// Uploads data to a GPU-accessible buffer using an upload heap.
// Returns a ComPtr to the created ID3D12Resource2 buffer.
// Note: For optimal performance, consider using a default heap and staging resource for large or frequent uploads.
ComPtr<ID3D12Resource2> UploadBuffer::Upload(void* data, uint32_t dataSize)
{
	// Describe the buffer resource
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

	// Create the committed resource in the upload heap
	ComPtr<ID3D12Resource2> uploadBuffer;
	CD3DX12_HEAP_PROPERTIES heapUploadProperties(D3D12_HEAP_TYPE_UPLOAD);
	ThrowIfFailed(
		GRHI.GetDevice()->CreateCommittedResource(
			&heapUploadProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())),
		"Failed to create upload buffer"
	);

	DebugUtils::SetDebugName(uploadBuffer, L"RHI_UploadBuffer");
	// Map the buffer and copy the data
	void* mappedData = nullptr;
	D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read from this resource on CPU
	ThrowIfFailed(
		uploadBuffer->Map(0, &readRange, &mappedData),
		"Failed To Map Upload Buffer"
	);
	memcpy(mappedData, data, dataSize);
	uploadBuffer->Unmap(0, nullptr);

	// TODO: For large or frequent uploads, use a default heap and a staging upload resource for best performance.

	return uploadBuffer;
}