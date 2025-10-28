#pragma once

#include "../Vendor/Windows/WinInclude.h"

static const UINT NumFramesInFlight = 3;

class SwapChain
{
public:
	void Initialize();
	void Resize();
	ID3D12Resource* GetBackBufferResource();
	void CreateRenderTargetViews();
	void ReleaseBuffers();
	void Shutdown();
	void Present();

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackbufferRTVHandle() { return m_rtvHandles[m_currentBufferIndex]; }
	UINT GetBackBufferIndex() { return m_currentBufferIndex; }
	void UpdateCurrentBackBufferIndex() { m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	D3D12_VIEWPORT GetDefaultViewport();
	D3D12_RECT GetDefaultScissorRect();

public:
	ComPointer<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
	UINT m_rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[NumFramesInFlight];
	UINT m_currentBufferIndex = 0;

	ComPointer<IDXGISwapChain3> m_swapChain = nullptr;
	ComPointer<ID3D12Resource2> m_buffers[NumFramesInFlight];
};

extern SwapChain GSwapChain; 

