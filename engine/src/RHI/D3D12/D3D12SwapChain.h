// ============================================================================
// D3D12SwapChain.h
// ----------------------------------------------------------------------------
// Manages the DXGI swap chain and associated render targets for presentation.
//
// USAGE:
//   GD3D12SwapChain.Initialize();
//   // Each frame:
//   GD3D12SwapChain.SetRenderTargetState();
//   GD3D12SwapChain.Clear();
//   // ... render ...
//   GD3D12SwapChain.SetPresentState();
//   GD3D12SwapChain.Present();
//   GD3D12SwapChain.Shutdown();
//
// DESIGN:
//   - Owns IDXGISwapChain3 and per-frame back buffer resources
//   - Supports variable-rate tearing and frame latency waitable objects
//   - Handles resize by releasing and recreating buffers
//
// NOTES:
//   - Singleton accessed via GD3D12SwapChain global reference
//   - Frame index updated via UpdateFrameInFlightIndex() after Present()
//   - Buffer count configured by EngineSettings::FramesInFlight
// ============================================================================

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

class D3D12SwapChain final
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Returns the singleton D3D12SwapChain instance.
	[[nodiscard]] static D3D12SwapChain& Get() noexcept;

	D3D12SwapChain(const D3D12SwapChain&) = delete;
	D3D12SwapChain& operator=(const D3D12SwapChain&) = delete;
	D3D12SwapChain(D3D12SwapChain&&) = delete;
	D3D12SwapChain& operator=(D3D12SwapChain&&) = delete;

	/// Creates the swap chain and render target views.
	void Initialize();

	/// Releases all swap chain resources.
	void Shutdown();

	// ========================================================================
	// Frame Operations
	// ========================================================================

	/// Presents the current back buffer to the screen.
	void Present();

	/// Clears the current render target view with the clear color.
	void Clear();

	/// Transitions current buffer to render target state.
	void SetRenderTargetState();

	/// Transitions current buffer to present state.
	void SetPresentState();

	/// Resizes swap chain buffers (called on window resize).
	void Resize();

	// ========================================================================
	// Accessors
	// ========================================================================

	/// Returns the CPU descriptor handle for a specific buffer index.
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const { return m_rtvHandles[index].GetCPU(); }

	/// Returns the CPU descriptor handle for the current back buffer.
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return GetCPUHandle(m_frameInFlightIndex); }

	/// Returns the current frame-in-flight index (0 to FramesInFlight-1).
	[[nodiscard]] UINT GetFrameInFlightIndex() const { return m_frameInFlightIndex; }

	/// Updates frame index from swap chain (call after Present).
	void UpdateFrameInFlightIndex() { m_frameInFlightIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	/// Returns the waitable object handle for frame pacing.
	[[nodiscard]] HANDLE GetWaitableObject() const { return m_WaitableObject; }

	/// Returns the default viewport matching window dimensions.
	[[nodiscard]] D3D12_VIEWPORT GetDefaultViewport() const;

	/// Returns the default scissor rect matching window dimensions.
	[[nodiscard]] D3D12_RECT GetDefaultScissorRect() const;

	/// Returns the back buffer format from engine settings.
	[[nodiscard]] DXGI_FORMAT GetBackBufferFormat() const { return EngineSettings::BackBufferFormat; }

	// ========================================================================
	// Feature Queries
	// ========================================================================

	/// Returns DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING if supported, else 0.
	[[nodiscard]] UINT GetAllowTearingFlag() const;

	/// Returns waitable object flag if multiple frames in flight.
	[[nodiscard]] UINT GetFrameLatencyWaitableFlag() const
	{
		return (EngineSettings::FramesInFlight > 1) ? DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : 0u;
	}

	/// Returns combined swap chain flags for creation.
	[[nodiscard]] UINT ComputeSwapChainFlags() const;

  private:
	D3D12SwapChain() = default;
	~D3D12SwapChain() = default;

	// ------------------------------------------------------------------------
	// Initialization Helpers
	// ------------------------------------------------------------------------

	void CreateRenderTargetViews();
	void AllocateHandles();
	void Create();
	void ReleaseBuffers();

	// ------------------------------------------------------------------------
	// State
	// ------------------------------------------------------------------------

	UINT m_frameInFlightIndex = 0;                                      ///< Current back buffer index
	ComPtr<IDXGISwapChain3> m_swapChain = nullptr;                       ///< DXGI swap chain
	ComPtr<ID3D12Resource2> m_buffers[EngineSettings::FramesInFlight];   ///< Back buffer resources
	D3D12DescriptorHandle m_rtvHandles[EngineSettings::FramesInFlight];  ///< RTV descriptor handles
	HANDLE m_WaitableObject = nullptr;                                   ///< Frame latency waitable object
};

/// Global singleton reference for convenient access.
inline D3D12SwapChain& GD3D12SwapChain = D3D12SwapChain::Get();
