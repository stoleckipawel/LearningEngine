#pragma once

#include "D3D12Rhi.h"
#include <cstddef>
#include <type_traits>

using Microsoft::WRL::ComPtr;

// UploadBuffer provides utilities for uploading small blobs to GPU memory using
// committed upload-heap resources. For large or frequent uploads prefer a
// ring/linear upload allocator or staging to a default heap and an explicit
// copy command (see engine design notes).
class D3D12UploadBuffer
{
  public:
	// Uploads the given data to a newly created upload-heap buffer and returns
	// a ComPtr to the GPU resource. `data` must point to at least `dataSize`
	// bytes. `dataSize` is size in bytes.
	static ComPtr<ID3D12Resource2> Upload(const void* data, size_t dataSize);
};
