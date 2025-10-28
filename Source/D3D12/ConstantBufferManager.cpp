#include "ConstantBufferManager.h"

ConstantBufferManager GConstantBufferManager;

void ConstantBufferManager::Initialize()
{
	for (size_t i = 0; i < FrameCount; ++i)
	{
		//Create Vertex Constant Buffer
		ConstantBuffer<FVertexConstantBufferData> vertexConstantBuffer;
		vertexConstantBuffer.Initialize(0);
		vertexConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetConstantBufferHeap().GetCPUHandle(i, vertexConstantBuffer.GetDescriptorHandleIndex()));
		VertexConstantBuffers[i] = vertexConstantBuffer;

		//Create Pixel Constant Buffer
		ConstantBuffer<PixelConstantBufferData> pixelConstantBuffer;
		pixelConstantBuffer.Initialize(1);
		pixelConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetConstantBufferHeap().GetCPUHandle(i, pixelConstantBuffer.GetDescriptorHandleIndex()));
		PixelConstantBuffers[i] = pixelConstantBuffer;
	}
}

void ConstantBufferManager::Update(size_t FrameIndex)
{
	float speed = FrameIndex * 0.02f; 

	//Update Vertex Constant Buffers
	FVertexConstantBufferData vertexData;
	vertexData.WorldMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	vertexData.ViewMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	vertexData.ProjectionMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	VertexConstantBuffers[GSwapChain.GetBackBufferIndex()].Update(vertexData);

	//Update Pixel Constant Buffers
	PixelConstantBufferData pixelData;
	pixelData.Color.x = 0.5f + 0.5f * sinf(speed);
	pixelData.Color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	pixelData.Color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	pixelData.Color.w = 1.0f;
	PixelConstantBuffers[GSwapChain.GetBackBufferIndex()].Update(pixelData);
}

void ConstantBufferManager::Release()
{
	for (size_t i = 0; i < FrameCount; ++i)
	{
		VertexConstantBuffers[i].Resource.Release();
		PixelConstantBuffers[i].Resource.Release();
	}
}