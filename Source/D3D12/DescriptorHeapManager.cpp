#include "DescriptorHeapManager.h"

DescriptorHeapManager GDescriptorHeapManager;

void DescriptorHeapManager::Initialize()
{
	UINT CBVCount = 2;
	UINT SRVCount = 1;
	UINT UAVCount = 0;
	m_CBVSRVUAVHeap.InitializeCBVSRVUAV(CBVCount, SRVCount, UAVCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"CBVSRVUAVHeap");
	m_SamplerHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"SamplerHeap");
	m_DepthStencilViewHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"DepthStencilHeap");
	m_RenderTargetViewHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NumFramesInFlight, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"RenderTargetHeap");
}

void DescriptorHeapManager::SetShaderVisibleHeaps()
{
	ID3D12DescriptorHeap* heaps[] = 
	{ 
		m_CBVSRVUAVHeap.Heap.Get(), 
		m_SamplerHeap.Heap.Get()
	};
 
	GRHI.GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}

void DescriptorHeapManager::Release()
{
	m_CBVSRVUAVHeap.Heap.Release();
	m_SamplerHeap.Heap.Release();
	m_DepthStencilViewHeap.Heap.Release();
	m_RenderTargetViewHeap.Heap.Release();
}