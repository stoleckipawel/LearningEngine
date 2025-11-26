#include "DescriptorHeapManager.h"

// Global instance of DescriptorHeapManager for engine-wide access
DescriptorHeapManager GDescriptorHeapManager;


// Initializes all descriptor heaps required by the engine
void DescriptorHeapManager::Initialize()
{
	// Number of descriptors for each heap type
	constexpr UINT CBVCount = 2; // Constant Buffer Views
	constexpr UINT SRVCount = 1; // Shader Resource Views
	constexpr UINT UAVCount = 0; // Unordered Access Views
	constexpr UINT SamplerCount = 1; // Samplers Views
	constexpr UINT DepthStencilCount = 1; // Depth Stencil Views

	// Initialize CBV/SRV/UAV heap (shader visible)
	m_CBVSRVUAVHeap.InitializeCBVSRVUAV(
		CBVCount,
		SRVCount,
		UAVCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"CBVSRVUAVHeap"
	);

	// Initialize Sampler heap (shader visible)
	m_SamplerHeap.Initialize(
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		SamplerCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"SamplerHeap"
	);

	// Initialize Depth Stencil View heap (not shader visible)
	m_DepthStencilViewHeap.Initialize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		DepthStencilCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		L"DepthStencilHeap"
	);

	// Initialize Render Target View heap (not shader visible)
	m_RenderTargetViewHeap.Initialize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		NumFramesInFlight,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		L"RenderTargetHeap"
	);
}


// Sets the shader-visible descriptor heaps for the current command list
void DescriptorHeapManager::SetShaderVisibleHeaps()
{
	ID3D12DescriptorHeap* heaps[] =
	{
		m_CBVSRVUAVHeap.Heap.Get(), // CBV/SRV/UAV heap
		m_SamplerHeap.Heap.Get()    // Sampler heap
	};

	// Bind the descriptor heaps to the command list
	GRHI.GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}


// Releases all descriptor heap resources
void DescriptorHeapManager::Release()
{
	m_CBVSRVUAVHeap.Heap.Release();
	m_SamplerHeap.Heap.Release();
	m_DepthStencilViewHeap.Heap.Release();
	m_RenderTargetViewHeap.Heap.Release();
}