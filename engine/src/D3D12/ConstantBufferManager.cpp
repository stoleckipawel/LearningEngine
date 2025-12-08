#include "PCH.h"
#include "D3D12/ConstantBufferManager.h"
#include "D3D12/Camera.h"

// Global constant buffer manager instance
ConstantBufferManager GConstantBufferManager;

// Initializes all constant buffers for each frame in flight
void ConstantBufferManager::Initialize()
{
	for (UINT FrameInFlightIndex = 0; FrameInFlightIndex < NumFramesInFlight; ++FrameInFlightIndex)
	{
		// Create and initialize constant buffers	
		VertexConstantBuffers[FrameInFlightIndex] = std::make_unique<ConstantBuffer<FVertexConstantBufferData>>();
		PixelConstantBuffers[FrameInFlightIndex] = std::make_unique<ConstantBuffer<PixelConstantBufferData>>();
	}
}

// Updates the constant buffers for the current frame
void ConstantBufferManager::Update(size_t FrameInFlightIndex)
{
	float speed = FrameInFlightIndex * 0.02f; 

	// Update vertex constant buffer with world, view, and projection matrices
	FVertexConstantBufferData vertexData;
	// Rotate diagonally around the (1,1,1) axis for 3D dimensionality
	XMVECTOR diagAxis = XMVector3Normalize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	float angle = XMConvertToRadians(speed * 100.0);

	XMMATRIX rotation = XMMatrixRotationAxis(diagAxis, angle);
	XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f); // Centered
	XMMATRIX world = scale * rotation * translation;
	XMStoreFloat4x4(&vertexData.WorldMTX, world);

	XMMATRIX view = GCamera.GetViewMatrix();
	XMStoreFloat4x4(&vertexData.ViewMTX, view);

	XMMATRIX projection = GCamera.GetProjectionMatrix();
	XMStoreFloat4x4(&vertexData.ProjectionMTX, projection);

	XMMATRIX worldViewProj = world * view * projection;
	XMStoreFloat4x4(&vertexData.WorldViewProjMTX, worldViewProj);
	
	VertexConstantBuffers[GSwapChain.GetFrameInFlightIndex()]->Update(vertexData);

	// Update pixel constant buffer with animated color
	PixelConstantBufferData pixelData;
	pixelData.Color.x = 0.5f + 0.5f * sinf(speed);
	pixelData.Color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	pixelData.Color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	pixelData.Color.w = 1.0f;
	PixelConstantBuffers[GSwapChain.GetFrameInFlightIndex()]->Update(pixelData);
}