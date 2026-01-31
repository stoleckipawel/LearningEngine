#include "PCH.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12FrameResource.h"
#include "RenderCamera.h"
#include "Timer.h"
#include "Window.h"
#include "UI.h"
#include "D3D12SwapChain.h"
#include <cmath>

D3D12ConstantBufferManager::D3D12ConstantBufferManager(
    Timer& timer,
    D3D12Rhi& rhi,
    Window& window,
    D3D12DescriptorHeapManager& descriptorHeapManager,
    D3D12FrameResourceManager& frameResourceManager,
    D3D12SwapChain& swapChain,
    UI& ui) :
    m_timer(&timer),
    m_window(&window),
    m_frameResourceManager(&frameResourceManager),
    m_swapChain(&swapChain),
    m_ui(&ui)
{
	for (uint32_t i = 0; i < EngineSettings::FramesInFlight; ++i)
	{
		m_PerFrameCB[i] = std::make_unique<D3D12ConstantBuffer<PerFrameConstantBufferData>>(rhi, descriptorHeapManager);
		m_PerViewCB[i] = std::make_unique<D3D12ConstantBuffer<PerViewConstantBufferData>>(rhi, descriptorHeapManager);
	}
}

D3D12ConstantBufferManager::~D3D12ConstantBufferManager()
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
	return m_PerFrameCB[m_swapChain->GetFrameInFlightIndex()]->GetGPUVirtualAddress();
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBufferManager::GetPerViewGpuAddress() const
{
	return m_PerViewCB[m_swapChain->GetFrameInFlightIndex()]->GetGPUVirtualAddress();
}

//------------------------------------------------------------------------------
// Per-Frame Update (once per CPU frame)
//------------------------------------------------------------------------------

void D3D12ConstantBufferManager::UpdatePerFrame()
{
	PerFrameConstantBufferData data = {};
	data.FrameIndex = m_timer->GetFrameCount();
	data.TotalTime = static_cast<float>(m_timer->GetTotalTime(TimeDomain::Unscaled, TimeUnit::Seconds));
	data.DeltaTime = static_cast<float>(m_timer->GetDelta(TimeDomain::Unscaled, TimeUnit::Seconds));
	data.ScaledTotalTime = static_cast<float>(m_timer->GetTotalTime(TimeDomain::Scaled, TimeUnit::Seconds));
	data.ScaledDeltaTime = static_cast<float>(m_timer->GetDelta(TimeDomain::Scaled, TimeUnit::Seconds));
	const float width = static_cast<float>(m_window->GetWidth());
	const float height = static_cast<float>(m_window->GetHeight());
	data.ViewportSize = DirectX::XMFLOAT2(width, height);
	data.ViewportSizeInv = DirectX::XMFLOAT2(width != 0.0f ? 1.0f / width : 0.0f, height != 0.0f ? 1.0f / height : 0.0f);
	data.ViewModeIndex = static_cast<uint32_t>(m_ui->GetViewMode());

	const uint32_t frameInFlightIndex = m_swapChain->GetFrameInFlightIndex();
	m_PerFrameCB[frameInFlightIndex]->Update(data);
}

//------------------------------------------------------------------------------
// Per-View Update (once per camera/view)
//------------------------------------------------------------------------------

void D3D12ConstantBufferManager::UpdatePerView(const RenderCamera& camera)
{
	const PerViewConstantBufferData data = camera.GetViewConstantBufferData();

	const uint32_t frameInFlightIndex = m_swapChain->GetFrameInFlightIndex();
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
	return m_frameResourceManager->AllocateConstantBuffer(data);
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
	return m_frameResourceManager->AllocateConstantBuffer(data);
}
