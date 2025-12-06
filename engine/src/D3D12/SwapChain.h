#include "PCH.h"

#pragma once
// Number of frames that can be processed simultaneously
static const UINT NumFramesInFlight = 2;

// SwapChain manages the Direct3D 12 swap chain and its associated render targets.
class SwapChain
{
public:
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

	// Returns the CPU descriptor handle for the specified buffer index
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index);
	// Returns the CPU descriptor handle for the current back buffer
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
	// Returns the current back buffer index
	UINT GetFrameInFlightIndex() { return m_frameInFlightIndex; }
	// Updates the current back buffer index from the swap chain
	void UpdateFrameInFlightIndex() { m_frameInFlightIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	// Returns the default viewport for rendering
	D3D12_VIEWPORT GetDefaultViewport();
	// Returns the default scissor rectangle for rendering
	D3D12_RECT GetDefaultScissorRect();
	// Returns the DXGI format used for back buffers
	DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; }

	// Feature flag helpers
	UINT GetAllowTearingFlag() const; // queries DXGI and returns DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING if supported
	UINT GetFrameLatencyWaitableFlag() const; // returns DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT when applicable
	UINT ComputeSwapChainFlags() const; // aggregates all feature flags
private:
	// Current back buffer index
	UINT m_frameInFlightIndex = 0;
	// Swap chain interface
	ComPointer<IDXGISwapChain3> m_swapChain = nullptr;
	// Array of render target resources (one per frame)
	ComPointer<ID3D12Resource2> m_buffers[NumFramesInFlight];
	// Back buffer format used by the swap chain
	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
private:
	// Creates render target views for all swap chain buffers
	void CreateRenderTargetViews();

	// Releases all buffer resources
	void ReleaseBuffers();
};


// Global swap chain instance
extern SwapChain GSwapChain;

