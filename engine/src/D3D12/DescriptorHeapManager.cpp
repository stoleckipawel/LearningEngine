#include "Core/PCH.h"
#include "D3D12/DescriptorHeapManager.h"

// Global instance of DescriptorHeapManager for engine-wide access
DescriptorHeapManager GDescriptorHeapManager;

// Initializes all descriptor heaps required by the engine
void DescriptorHeapManager::Initialize()
{
	// Number of descriptors for each heap type
	constexpr UINT CBVCount = 2; // Constant Buffer Views
	constexpr UINT SRVCount = 2; // Shader Resource Views
	constexpr UINT SamplerCount = 1; // Samplers Views
	constexpr UINT DepthStencilCount = 1; // Depth Stencil Views

	// Create CBV/SRV/UAV heap (shader visible)
	m_CBVSRVUAVHeap = std::make_unique<DescriptorHeap>(
		CBVCount,
		SRVCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"CBVSRVUAVHeap"
	);

	// Create Sampler heap (shader visible)
	m_SamplerHeap = std::make_unique<DescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		SamplerCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"SamplerHeap"
	);

	// Create Depth Stencil View heap (not shader visible)
	m_DepthStencilViewHeap = std::make_unique<DescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		DepthStencilCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		L"DepthStencilHeap"
	);

	// Create Render Target View heap (not shader visible)
	m_RenderTargetViewHeap = std::make_unique<DescriptorHeap>(
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
		m_CBVSRVUAVHeap->GetRaw(), // CBV/SRV/UAV heap
		m_SamplerHeap->GetRaw()    // Sampler heap
	};

	// Bind the descriptor heaps to the command list
	GRHI.GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}


// Releases all descriptor heap resources
void DescriptorHeapManager::Release()
{
	m_CBVSRVUAVHeap.reset();
	m_SamplerHeap.reset();
	m_DepthStencilViewHeap.reset();
	m_RenderTargetViewHeap.reset();
}