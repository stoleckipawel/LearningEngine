#pragma once

#include "../Vendor/Windows/WinInclude.h"

static const UINT NumFramesInFlight = 3;

class SwapChain
{
public:
	void Initialize();
	void Shutdown();
	void Present();
	void Clear();
	void SetRenderTargetState();
	void SetPresentState();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
	UINT GetBackBufferIndex() { return m_currentBufferIndex; }
	void UpdateCurrentBackBufferIndex() { m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex(); }

	D3D12_VIEWPORT GetDefaultViewport();
	D3D12_RECT GetDefaultScissorRect();
private:
	UINT m_currentBufferIndex = 0;
	ComPointer<IDXGISwapChain3> m_swapChain = nullptr;
	ComPointer<ID3D12Resource2> m_buffers[NumFramesInFlight];
private:
	void CreateRenderTargetViews();
	void Resize();
	void ReleaseBuffers();
};

extern SwapChain GSwapChain; 

