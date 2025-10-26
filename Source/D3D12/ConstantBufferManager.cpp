#include "ConstantBufferManager.h"

ConstantBufferManager GConstantBufferManager;

void ConstantBufferManager::Initialize()
{
	for (size_t i = 0; i < BufferingCount; ++i)
	{
		//Create Vertex Constant Buffer
		FConstantBuffer<FVertexConstantBuffer> vertexConstantBuffer;
		vertexConstantBuffer.Initialize(0);
		vertexConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetConstantBufferHeap().GetCPUHandle(i, vertexConstantBuffer.HandleIndex));
		VertexConstantBuffers[i] = vertexConstantBuffer;

		//Create Pixel Constant Buffer
		FConstantBuffer<FPixelConstantBuffer> pixelConstantBuffer;
		pixelConstantBuffer.Initialize(1);
		pixelConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetConstantBufferHeap().GetCPUHandle(i, pixelConstantBuffer.HandleIndex));
		PixelConstantBuffers[i] = pixelConstantBuffer;
	}
}

void ConstantBufferManager::UpdateVertexConstantBuffer(const FVertexConstantBuffer& data)
{
	VertexConstantBuffers[GSwapChain.GetCurrentBackBufferIndex()].Update(data);
}

void ConstantBufferManager::UpdatePixelConstantBuffer(const FPixelConstantBuffer& data)
{
	PixelConstantBuffers[GSwapChain.GetCurrentBackBufferIndex()].Update(data);
}

void ConstantBufferManager::Release()
{
	for (size_t i = 0; i < BufferingCount; ++i)
	{
		VertexConstantBuffers[i].Resource.Release();
		PixelConstantBuffers[i].Resource.Release();
	}
}