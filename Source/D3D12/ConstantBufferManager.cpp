#include "ConstantBufferManager.h"
#include "Camera.h"

ConstantBufferManager GConstantBufferManager;

void ConstantBufferManager::Initialize()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		//Create Vertex Constant Buffer
		ConstantBuffer<FVertexConstantBufferData> vertexConstantBuffer;
		vertexConstantBuffer.Initialize(0);
		vertexConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVCPUHandle(vertexConstantBuffer.GetDescriptorHandleIndex(), i));
		VertexConstantBuffers[i] = vertexConstantBuffer;

		//Create Pixel Constant Buffer
		ConstantBuffer<PixelConstantBufferData> pixelConstantBuffer;
		pixelConstantBuffer.Initialize(1);
		pixelConstantBuffer.CreateConstantBufferView(GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVCPUHandle(pixelConstantBuffer.GetDescriptorHandleIndex(), i));
		PixelConstantBuffers[i] = pixelConstantBuffer;
	}
}

void ConstantBufferManager::Update(size_t FrameIndex)
{
	float speed = FrameIndex * 0.02f; 

	//Update Vertex Constant Buffers
	FVertexConstantBufferData vertexData;
	float angleRadians = XMConvertToRadians(FrameIndex);
	XMMATRIX world = XMMatrixRotationZ(angleRadians);
	XMStoreFloat4x4(&vertexData.WorldMTX, world);

	XMStoreFloat4x4(&vertexData.ViewMTX, GCamera.GetViewMatrix());
	XMStoreFloat4x4(&vertexData.ProjectionMTX, GCamera.GetProjectionMatrix());
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
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		VertexConstantBuffers[i].Resource.Release();
		PixelConstantBuffers[i].Resource.Release();
	}
}