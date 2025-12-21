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
#include "DescriptorHandle.h"
#include "EngineConfig.h"

using Microsoft::WRL::ComPtr;

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
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) { return m_rtvHandles[index].GetCPU(); }
	// Returns the CPU descriptor handle for the current back buffer
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() { return GetCPUHandle(m_frameInFlightIndex); }
	// Returns the current back buffer index
	UINT GetFrameInFlightIndex() { return m_frameInFlightIndex; }
	// Updates the current back buffer index from the swap chain
	void UpdateFrameInFlightIndex() { m_frameInFlightIndex = m_swapChain->GetCurrentBackBufferIndex(); }
	HANDLE GetWaitableObject() const { return m_WaitableObject; }

	// Returns the default viewport for rendering
	D3D12_VIEWPORT GetDefaultViewport();
	// Returns the default scissor rectangle for rendering
	D3D12_RECT GetDefaultScissorRect();
	// Returns the DXGI format used for back buffers
	DXGI_FORMAT GetBackBufferFormat() const { return EngineSettings::BackBufferFormat; }

	// Feature flag helpers
	UINT GetAllowTearingFlag() const; // queries DXGI and returns DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING if supported
	// Returns DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT when applicable
	UINT GetFrameLatencyWaitableFlag() const { return (EngineSettings::FramesInFlight > 1) ? DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT : 0u; }
	UINT ComputeSwapChainFlags() const; // aggregates all feature flags
private:
	// Current back buffer index
	UINT m_frameInFlightIndex = 0;
	// Swap chain interface
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain = nullptr;
	// Array of render target resources (one per frame)
	Microsoft::WRL::ComPtr<ID3D12Resource2> m_buffers[EngineSettings::FramesInFlight];
	// Array of RTV descriptor handles (allocated via manager)
	DescriptorHandle m_rtvHandles[EngineSettings::FramesInFlight];
	HANDLE m_WaitableObject = nullptr;	
private:
	// Creates render target views for all swap chain buffers
	void CreateRenderTargetViews();
	void AllocateHandles();
	void Create();

	// Releases all buffer resources
	void ReleaseBuffers();
};


// Global swap chain instance
extern SwapChain GSwapChain;

