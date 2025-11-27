#pragma once

#include "RHI.h"
#include "ConstantBuffer.h"

// Vertex constant buffer data (aligned to 256 bytes for D3D12)
struct alignas(256) FVertexConstantBufferData
{
	XMFLOAT4X4 WorldMTX;      // World transformation matrix
	XMFLOAT4X4 ViewMTX;       // View transformation matrix
	XMFLOAT4X4 ProjectionMTX; // Projection transformation matrix
};

// Pixel constant buffer data (aligned to 256 bytes for D3D12)
struct alignas(256) PixelConstantBufferData
{
	XMFLOAT4 Color;           // RGBA color
};


// ConstantBufferManager manages per-frame vertex and pixel constant buffers for rendering.
class ConstantBufferManager
{
public:
	// Initializes all constant buffers for each frame in flight
	void Initialize();

	UINT GetDescriptorHandleIndex(UINT ConstantBufferID, UINT GroupIndex);

	// Releases all constant buffers
	void Release();
	// Updates the constant buffers
	void Update(size_t FrameIndex);

	// Per-frame vertex constant buffers
	std::unique_ptr<ConstantBuffer<FVertexConstantBufferData>> VertexConstantBuffers[NumFramesInFlight];
	// Per-frame pixel constant buffers
	std::unique_ptr<ConstantBuffer<PixelConstantBufferData>> PixelConstantBuffers[NumFramesInFlight];
};


// Global constant buffer manager instance
extern ConstantBufferManager GConstantBufferManager;