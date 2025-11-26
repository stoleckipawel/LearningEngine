
#include "ConstantBufferManager.h"
#include "Camera.h"

// Global constant buffer manager instance
ConstantBufferManager GConstantBufferManager;

// Initializes all constant buffers for each frame in flight
void ConstantBufferManager::Initialize()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		// Create and initialize vertex constant buffer (RAII, unique_ptr)
		VertexConstantBuffers[i] = std::make_unique<ConstantBuffer<FVertexConstantBufferData>>(0);
		VertexConstantBuffers[i]->CreateConstantBufferView(GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVCPUHandle(VertexConstantBuffers[i]->GetDescriptorHandleIndex(), i));

		// Create and initialize pixel constant buffer (RAII, unique_ptr)
		PixelConstantBuffers[i] = std::make_unique<ConstantBuffer<PixelConstantBufferData>>(1);
		PixelConstantBuffers[i]->CreateConstantBufferView(GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVCPUHandle(PixelConstantBuffers[i]->GetDescriptorHandleIndex(), i));
	}
}

// Updates the constant buffers for the current frame
void ConstantBufferManager::Update(size_t FrameIndex)
{
	float speed = FrameIndex * 0.02f; 

	// Update vertex constant buffer with world, view, and projection matrices
	FVertexConstantBufferData vertexData;
	float angleX = XMConvertToRadians(FrameIndex * 1.0f); // X axis speed
	float angleY = XMConvertToRadians(FrameIndex * 0.7f); // Y axis speed
	float angleZ = XMConvertToRadians(FrameIndex * 1.3f); // Z axis speed
	XMMATRIX world = XMMatrixRotationX(angleX) * XMMatrixRotationY(angleY) * XMMatrixRotationZ(angleZ);
	XMStoreFloat4x4(&vertexData.WorldMTX, world);
	XMStoreFloat4x4(&vertexData.ViewMTX, GCamera.GetViewMatrix());
	XMStoreFloat4x4(&vertexData.ProjectionMTX, GCamera.GetProjectionMatrix());
	VertexConstantBuffers[GSwapChain.GetBackBufferIndex()]->Update(vertexData);

	// Update pixel constant buffer with animated color
	PixelConstantBufferData pixelData;
	pixelData.Color.x = 0.5f + 0.5f * sinf(speed);
	pixelData.Color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	pixelData.Color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	pixelData.Color.w = 1.0f;
	PixelConstantBuffers[GSwapChain.GetBackBufferIndex()]->Update(pixelData);
}

// Releases all constant buffers
void ConstantBufferManager::Release()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i)
	{
		VertexConstantBuffers[i].reset();
		PixelConstantBuffers[i].reset();
	}
}
