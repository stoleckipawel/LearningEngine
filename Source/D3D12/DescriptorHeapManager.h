#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "DescriptorHeap.h"

class DescriptorHeapManager
{
public:
	void Initialize();
	void SetShaderVisibleHeaps();
	void Release();

	DescriptorHeap& GetCBVSRVUAVHeap() { return m_CBVSRVUAVHeap; }
	DescriptorHeap& GetSamplerHeap() { return m_SamplerHeap; }
	DescriptorHeap& GetDepthStencilViewHeap() { return m_DepthStencilViewHeap; }
	DescriptorHeap& GetRenderTargetViewHeap() { return m_RenderTargetViewHeap; }
private:
	DescriptorHeap m_CBVSRVUAVHeap;
	DescriptorHeap m_SamplerHeap;
	DescriptorHeap m_DepthStencilViewHeap;
	DescriptorHeap m_RenderTargetViewHeap;
};

extern DescriptorHeapManager GDescriptorHeapManager;