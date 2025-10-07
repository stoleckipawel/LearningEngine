#pragma once

#include "../Vendor/Windows/WinInclude.h"

static const UINT BufferingCount = 2;

class FSwapChain
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
	UINT GetCurrentBackBufferIndex() { return m_currentBufferIndex; }
	void UpdateCurrentBackBufferIndex() { m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	D3D12_VIEWPORT GetDefaultViewport();
	D3D12_RECT GetDefaultScissorRect();

public:
	static constexpr UINT BufferingCount = 3;;//2:Vsync OFF, 3: Vsync ON
	ComPointer<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
	UINT m_rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[BufferingCount];
	UINT m_currentBufferIndex = 0;

	ComPointer<IDXGISwapChain3> m_swapChain = nullptr;
	ComPointer<ID3D12Resource2> m_buffers[BufferingCount];
};

extern FSwapChain GSwapChain; 

