#include "PCH.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12FrameResource.h"
#include "Camera.h"
#include "Timer.h"
#include "Window.h"
#include "D3D12SwapChain.h"
#include <cmath>

D3D12ConstantBufferManager GD3D12ConstantBufferManager;

//------------------------------------------------------------------------------
// Initialization
//------------------------------------------------------------------------------

void D3D12ConstantBufferManager::Initialize()
{
	// Create persistent per-frame and per-view constant buffers
	// These are updated once per frame and don't need ring buffer allocation
	for (uint32_t i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_PerFrameCB[i] = std::make_unique<D3D12ConstantBuffer<PerFrameConstantBufferData>>();
		m_PerViewCB[i] = std::make_unique<D3D12ConstantBuffer<PerViewConstantBufferData>>();
	}

	// Per-object CBs are allocated dynamically from GFrameResourceManager
	// which is initialized by the Renderer before constant buffer updates
}

void D3D12ConstantBufferManager::Shutdown()
{
	for (uint32_t i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_PerFrameCB[i].reset();
		m_PerViewCB[i].reset();
	}
}

//------------------------------------------------------------------------------
// GPU Address Accessors
//------------------------------------------------------------------------------

D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBufferManager::GetPerFrameGpuAddress() const
{
	return m_PerFrameCB[GD3D12SwapChain.GetFrameInFlightIndex()]->GetGPUVirtualAddress();
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBufferManager::GetPerViewGpuAddress() const
{
	return m_PerViewCB[GD3D12SwapChain.GetFrameInFlightIndex()]->GetGPUVirtualAddress();
}

//------------------------------------------------------------------------------
// Per-Frame Update (once per CPU frame)
//------------------------------------------------------------------------------

void D3D12ConstantBufferManager::UpdatePerFrame()
{
	PerFrameConstantBufferData data = {};
	data.FrameIndex = GTimer.GetFrameCount();
	data.TotalTime = GTimer.GetTotalTime();
	data.DeltaTime = GTimer.GetDelta();
	data.ViewportSize = GWindow.GetViewportSize();
	data.ViewportSizeInv = GWindow.GetViewportSizeInv();

	const uint32_t frameInFlightIndex = GD3D12SwapChain.GetFrameInFlightIndex();
	m_PerFrameCB[frameInFlightIndex]->Update(data);
}

//------------------------------------------------------------------------------
// Per-View Update (once per camera/view)
//------------------------------------------------------------------------------

void D3D12ConstantBufferManager::UpdatePerView()
{
	PerViewConstantBufferData data = {};
	data.CameraPosition = GCamera.GetPosition();
	data.CameraDirection = GCamera.GetDirection();
	data.NearZ = GCamera.GetNearZ();
	data.FarZ = GCamera.GetFarZ();

	const XMMATRIX view = GCamera.GetViewMatrix();
	const XMMATRIX proj = GCamera.GetProjectionMatrix();
	const XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	XMStoreFloat4x4(&data.ViewMTX, view);
	XMStoreFloat4x4(&data.ProjectionMTX, proj);
	XMStoreFloat4x4(&data.ViewProjMTX, viewProj);

	const uint32_t frameInFlightIndex = GD3D12SwapChain.GetFrameInFlightIndex();
	m_PerViewCB[frameInFlightIndex]->Update(data);
}

//------------------------------------------------------------------------------
// Per-Object VS Update (per draw call - uses ring buffer)
//------------------------------------------------------------------------------
// This is the critical path for scaling to many objects:
//   - Each call allocates from the per-frame linear allocator
//   - Returns a unique GPU VA that won't be overwritten until next frame
//   - Thread-safe allocation allows future multithreaded recording
//------------------------------------------------------------------------------

D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBufferManager::UpdatePerObjectVS(const PerObjectVSConstantBufferData& data)
{
	// Allocate from ring buffer and copy data - returns unique GPU VA
	return GD3D12FrameResourceManager.AllocateConstantBuffer(data);
}

//------------------------------------------------------------------------------
// Per-Object PS Update (per draw call - uses ring buffer)
//------------------------------------------------------------------------------

D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBufferManager::UpdatePerObjectPS()
{
	PerObjectPSConstantBufferData data = {};
	data.BaseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	data.Metallic = 0.0f;
	data.Roughness = 0.5f;
	data.F0 = 0.04f;  // Typical dielectric F0
	data._padPerObjectPS0 = 0.0f;

	// Allocate from ring buffer and copy data - returns unique GPU VA
	return GD3D12FrameResourceManager.AllocateConstantBuffer(data);
}
