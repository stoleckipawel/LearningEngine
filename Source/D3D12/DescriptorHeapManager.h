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

	DescriptorHeap& GetCBVSRVUAVHeap() { return CBVSRVUAVHeap; }
	DescriptorHeap& GetSamplerHeap() { return SamplerHeap; }
	DescriptorHeap& GetDepthStencilViewHeap() { return DepthStencilViewHeap; }
private:
	DescriptorHeap CBVSRVUAVHeap;
	DescriptorHeap SamplerHeap;
	DescriptorHeap DepthStencilViewHeap;
	//RTV Heap:ToDo:
};

extern DescriptorHeapManager GDescriptorHeapManager;