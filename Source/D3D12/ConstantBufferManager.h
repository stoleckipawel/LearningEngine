#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "ConstantBuffer.h"
#include "DescriptorHeapManager.h"
#include "SwapChain.h"

struct alignas(256) FVertexConstantBuffer
{
	XMFLOAT4X4 WorldMTX;
	XMFLOAT4X4 ViewMTX;
	XMFLOAT4X4 ProjectionMTX;
};

struct alignas(256) FPixelConstantBuffer
{
	XMFLOAT4 Color;
};

class ConstantBufferManager
{
public:
	void Initialize();
	void Release();
	void Update(size_t FrameIndex);
private:
	void UpdateVertexConstantBuffer(const FVertexConstantBuffer& data);
	void UpdatePixelConstantBuffer(const FPixelConstantBuffer& data);
public:
	FConstantBuffer<FVertexConstantBuffer> VertexConstantBuffers[BufferingCount];
	FConstantBuffer<FPixelConstantBuffer> PixelConstantBuffers[BufferingCount];
};

extern ConstantBufferManager GConstantBufferManager;