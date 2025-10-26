#include "ConstantBufferManager.h"

ConstantBufferManager GConstantBufferManager;

void ConstantBufferManager::Initialize()
{
	for (size_t i = 0; i < BufferingCount; ++i)
	{
		//Create Vertex Constant Buffer
		FConstantBuffer<FVertexConstantBuffer> vertexConstantBuffer;
		vertexConstantBuffer.Initialize(0);
		vertexConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetConstantBufferHeap().GetCPUHandle(i, vertexConstantBuffer.DescriptorHandleIndex));
		VertexConstantBuffers[i] = vertexConstantBuffer;

		//Create Pixel Constant Buffer
		FConstantBuffer<FPixelConstantBuffer> pixelConstantBuffer;
		pixelConstantBuffer.Initialize(1);
		pixelConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetConstantBufferHeap().GetCPUHandle(i, pixelConstantBuffer.DescriptorHandleIndex));
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

void ConstantBufferManager::Update(size_t FrameIndex)
{
	float speed = FrameIndex * 0.02f; 

	//Update Vertex Constant Buffers
	FVertexConstantBuffer vertexData;
	vertexData.WorldMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	vertexData.ViewMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	vertexData.ProjectionMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	GConstantBufferManager.UpdateVertexConstantBuffer(vertexData);

	//Update Pixel Constant Buffers
	FPixelConstantBuffer pixelData;
	pixelData.Color.x = 0.5f + 0.5f * sinf(speed);
	pixelData.Color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	pixelData.Color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	pixelData.Color.w = 1.0f;
	GConstantBufferManager.UpdatePixelConstantBuffer(pixelData);	
}

void ConstantBufferManager::Release()
{
	for (size_t i = 0; i < BufferingCount; ++i)
	{
		VertexConstantBuffers[i].Resource.Release();
		PixelConstantBuffers[i].Resource.Release();
	}
}