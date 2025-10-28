#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "ConstantBuffer.h"
#include "DescriptorHeapManager.h"
#include "SwapChain.h"

struct alignas(256) FVertexConstantBufferData
{
	XMFLOAT4X4 WorldMTX;
	XMFLOAT4X4 ViewMTX;
	XMFLOAT4X4 ProjectionMTX;
};

struct alignas(256) PixelConstantBufferData
{
	XMFLOAT4 Color;
};

class ConstantBufferManager
{
public:
	void Initialize();
	void Release();
	void Update(size_t BackBufferFrameIndex);
public:
	ConstantBuffer<FVertexConstantBufferData> VertexConstantBuffers[NumFramesInFlight];
	ConstantBuffer<PixelConstantBufferData> PixelConstantBuffers[NumFramesInFlight];

};

extern ConstantBufferManager GConstantBufferManager;