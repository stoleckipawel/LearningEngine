#include "DescriptorHeapManager.h"

DescriptorHeapManager GDescriptorHeapManager;

void DescriptorHeapManager::Initialize()
{
	ConstantBufferHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, true, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"ConstantBufferHeap");
	TextureHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, false, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"TextureHeap");
	SamplerHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1, false, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"SamplerHeap");
	DepthStencilViewHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"DepthStencilHeap");
}

void DescriptorHeapManager::SetShaderVisibleHeaps()
{
	ID3D12DescriptorHeap* heaps[] = 
	{ 
		ConstantBufferHeap.heap.Get(), 
		SamplerHeap.heap.Get()
	};

	GRHI.GetCurrentCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}

void DescriptorHeapManager::Release()
{
	ConstantBufferHeap.heap.Release();
	TextureHeap.heap.Release();
	SamplerHeap.heap.Release();
	DepthStencilViewHeap.heap.Release();
}