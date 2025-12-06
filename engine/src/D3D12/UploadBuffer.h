#pragma once

#include "RHI.h"

// UploadBuffer provides a utility for uploading data to GPU-accessible buffers using an upload heap.
class UploadBuffer
{
public:
	// Uploads the given data to a buffer of the specified size.
	// Returns a ComPtr to the created ID3D12Resource2 buffer.
	static Microsoft::WRL::ComPtr<ID3D12Resource2> Upload(void* data, uint32_t dataSize);
};
