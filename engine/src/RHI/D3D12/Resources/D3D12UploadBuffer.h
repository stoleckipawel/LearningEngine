// ============================================================================
// D3D12UploadBuffer.h
// ----------------------------------------------------------------------------
// Utility for uploading small data blobs to GPU memory.
//
// USAGE:
//   auto resource = D3D12UploadBuffer::Upload(data, dataSize);
//
// DESIGN:
//   - Creates committed upload-heap resources for each upload
//   - Suitable for small, infrequent uploads (textures, initial buffers)
//
// PERFORMANCE NOTE:
//   For large or frequent uploads, prefer:
//   - LinearAllocator for per-frame constant buffers
//   - Staging to default heap with explicit copy commands
// ============================================================================

#pragma once

#include "D3D12Rhi.h"
#include <cstddef>
#include <type_traits>

using Microsoft::WRL::ComPtr;

class D3D12UploadBuffer
{
  public:
	// Uploads the given data to a newly created upload-heap buffer and returns
	// a ComPtr to the GPU resource. `data` must point to at least `dataSize`
	// bytes. `dataSize` is size in bytes.
	static ComPtr<ID3D12Resource2> Upload(const void* data, size_t dataSize);
};
