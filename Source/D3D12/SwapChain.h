#pragma once

#include "../Vendor/Windows/WinInclude.h"


// Number of frames that can be processed simultaneously
static const UINT NumFramesInFlight = 3;


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

	// Returns the CPU descriptor handle for the specified buffer index
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index);
	// Returns the CPU descriptor handle for the current back buffer
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
	// Returns the current back buffer index
	UINT GetBackBufferIndex() { return m_currentBufferIndex; }
	// Updates the current back buffer index from the swap chain
	void UpdateCurrentBackBufferIndex() { m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	// Returns the default viewport for rendering
	D3D12_VIEWPORT GetDefaultViewport();
	// Returns the default scissor rectangle for rendering
	D3D12_RECT GetDefaultScissorRect();
private:
	// Current back buffer index
	UINT m_currentBufferIndex = 0;
	// Swap chain interface
	ComPointer<IDXGISwapChain3> m_swapChain = nullptr;
	// Array of render target resources (one per frame)
	ComPointer<ID3D12Resource2> m_buffers[NumFramesInFlight];
private:
	// Creates render target views for all swap chain buffers
	void CreateRenderTargetViews();
	// Resizes the swap chain buffers
	void Resize();
	// Releases all buffer resources
	void ReleaseBuffers();
};


// Global swap chain instance
extern SwapChain GSwapChain;

