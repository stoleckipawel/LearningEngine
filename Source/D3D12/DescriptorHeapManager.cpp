#include "DescriptorHeapManager.h"

DescriptorHeapManager GDescriptorHeapManager;

void DescriptorHeapManager::Initialize()
{
	UINT CBVCount = 2;
	UINT SRVCount = 1;
	UINT UAVCount = 0;
	CBVSRVUAVHeap.InitializeCBVSRVUAV(CBVCount, SRVCount, UAVCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"CBVSRVUAVHeap");
	SamplerHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"SamplerHeap");
	DepthStencilViewHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"DepthStencilHeap");
}

void DescriptorHeapManager::SetShaderVisibleHeaps()
{
	ID3D12DescriptorHeap* heaps[] = 
	{ 
		CBVSRVUAVHeap.heap.Get(), 
		SamplerHeap.heap.Get()
	};
 
	GRHI.GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}

void DescriptorHeapManager::Release()
{
	CBVSRVUAVHeap.heap.Release();
	SamplerHeap.heap.Release();
	DepthStencilViewHeap.heap.Release();
}