#pragma once

#include "../Vendor/Windows/WinInclude.h"

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

	UINT GetBufferingCount() { return m_BufferingCount; };
	D3D12_CPU_DESCRIPTOR_HANDLE GetBackbufferRTVHandle() { return m_rtvHandles[m_currentBufferIndex]; }
	inline UINT GetBackBufferIndex() { return m_currentBufferIndex; }

	D3D12_VIEWPORT GetDefaultViewport();
	D3D12_RECT GetDefaultScissorRect();

public:
	static constexpr UINT m_BufferingCount = 3;;//2:Vsync OFF, 3: Vsync ON
	ComPointer<ID3D12DescriptorHeap> m_rtvDescHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[m_BufferingCount];
	UINT m_currentBufferIndex = 0;

	ComPointer<IDXGISwapChain3> m_swapChain = nullptr;
	ComPointer<ID3D12Resource2> m_buffers[m_BufferingCount];
};

extern FSwapChain GSwapChain; 

