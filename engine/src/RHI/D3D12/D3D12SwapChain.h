#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "D3D12DescriptorHandle.h"
#include "EngineConfig.h"

using Microsoft::WRL::ComPtr;

// SwapChain manages the Direct3D 12 swap chain and its associated render targets.
class D3D12SwapChain final
{
  public:
	[[nodiscard]] static D3D12SwapChain& Get() noexcept;

	D3D12SwapChain(const D3D12SwapChain&) = delete;
	D3D12SwapChain& operator=(const D3D12SwapChain&) = delete;
	D3D12SwapChain(D3D12SwapChain&&) = delete;
	D3D12SwapChain& operator=(D3D12SwapChain&&) = delete;

	// Initializes the swap chain and creates render target views
	void Initialize();
	// Releases all resources associated with the swap chain
	void Shutdown();
	// Presents the current back buffer to the screen
	void Present();
	// Clears the current render target view
	void Clear();

	// Sets the current buffer to render target state
	void SetRenderTargetState();
	// Sets the current buffer to present state
	void SetPresentState();

	// Resizes the swap chain buffers
	void Resize();

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const { return m_rtvHandles[index].GetCPU(); }
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return GetCPUHandle(m_frameInFlightIndex); }

	[[nodiscard]] UINT GetFrameInFlightIndex() const { return m_frameInFlightIndex; }
	void UpdateFrameInFlightIndex() { m_frameInFlightIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	[[nodiscard]] HANDLE GetWaitableObject() const { return m_WaitableObject; }

	[[nodiscard]] D3D12_VIEWPORT GetDefaultViewport() const;
	[[nodiscard]] D3D12_RECT GetDefaultScissorRect() const;
	[[nodiscard]] DXGI_FORMAT GetBackBufferFormat() const { return EngineSettings::BackBufferFormat; }

	[[nodiscard]] UINT GetAllowTearingFlag() const;  // queries DXGI and returns DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING if supported
	[[nodiscard]] UINT GetFrameLatencyWaitableFlag() const
	{
		return (EngineSettings::FramesInFlight > 1) ? DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : 0u;
	}
	[[nodiscard]] UINT ComputeSwapChainFlags() const;  // aggregates all feature flags

  private:
	D3D12SwapChain() = default;
	~D3D12SwapChain() = default;

	void CreateRenderTargetViews();
	void AllocateHandles();
	void Create();
	void ReleaseBuffers();

	UINT m_frameInFlightIndex = 0;
	ComPtr<IDXGISwapChain3> m_swapChain = nullptr;
	ComPtr<ID3D12Resource2> m_buffers[EngineSettings::FramesInFlight];
	D3D12DescriptorHandle m_rtvHandles[EngineSettings::FramesInFlight];
	HANDLE m_WaitableObject = nullptr;
};

inline D3D12SwapChain& GD3D12SwapChain = D3D12SwapChain::Get();
