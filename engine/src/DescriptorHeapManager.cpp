#include "PCH.h"
#include "DescriptorHeapManager.h"

// Global instance of DescriptorHeapManager for engine-wide access
DescriptorHeapManager GDescriptorHeapManager;

// Initializes all descriptor heaps required by the engine.
void DescriptorHeapManager::Initialize()
{
	// Create CBV/SRV/UAV heap (shader visible)
	m_HeapSRV = std::make_unique<DescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"CBVSRVUAVHeap"
	);
	// Initialize SRV allocator to manage SRV indices within the unified CBV/SRV/UAV heap
	m_AllocatorSRV = std::make_unique<DescriptorAllocator>(m_HeapSRV.get());

	// Create Sampler heap (shader visible)
	m_HeapSampler = std::make_unique<DescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		L"SamplerHeap"
	);
	// Initialize Sampler allocator
	m_AllocatorSampler = std::make_unique<DescriptorAllocator>(m_HeapSampler.get());

	// Create Depth Stencil View heap (not shader visible)
	m_HeapDepthStencil = std::make_unique<DescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		L"DepthStencilHeap"
	);
	// Initialize DSV allocator
	m_AllocatorDepthStencil = std::make_unique<DescriptorAllocator>(m_HeapDepthStencil.get());

	// Create Render Target View heap (not shader visible)
	m_HeapRenderTarget = std::make_unique<DescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		L"RenderTargetHeap"
	);
	// Initialize RTV allocator
	m_AllocatorRenderTarget = std::make_unique<DescriptorAllocator>(m_HeapRenderTarget.get());
}

// Binds shader-visible heaps (SRV/CBV/UAV and Sampler) on the command list.
void DescriptorHeapManager::SetShaderVisibleHeaps() const
{
	ID3D12DescriptorHeap* heaps[] =
	{
		m_HeapSRV->GetRaw(), // CBV/SRV/UAV heap
		m_HeapSampler->GetRaw()    // Sampler heap (optional for UI; harmless)
	};

	GRHI.GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}

// Allocates a descriptor of the given type and returns raw CPU/GPU handles.
void DescriptorHeapManager::AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE& outCPU, D3D12_GPU_DESCRIPTOR_HANDLE& outGPU)
{
	DescriptorAllocator* allocator = GetAllocator(type);
	const DescriptorHandle handle = allocator->Allocate();
	outCPU = handle.GetCPU();
	outGPU = handle.GetGPU();
}

// Frees a descriptor of the given type using raw CPU/GPU handles.
void DescriptorHeapManager::FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	DescriptorHeap* heap = GetHeap(type);
	DescriptorAllocator* allocator = GetAllocator(type);

	if (!heap || !allocator)
	{
		LOG_FATAL("FreeHandle: invalid heap or allocator");
	}

	// Compute index from CPU handle pointer arithmetic against the heap's CPU start
	const auto heapCPUStart = heap->GetRaw()->GetCPUDescriptorHandleForHeapStart();
	const UINT increment = GRHI.GetDevice()->GetDescriptorHandleIncrementSize(type);
	const SIZE_T delta = (cpuHandle.ptr - heapCPUStart.ptr);
	const UINT index = static_cast<UINT>(delta / static_cast<SIZE_T>(increment));

	// Only request GPU start for shader-visible heaps; otherwise pass a zeroed GPU handle.
	D3D12_GPU_DESCRIPTOR_HANDLE heapGPUStart = { 0 };
	const auto heapDesc = heap->GetRaw()->GetDesc();
	if (heapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		heapGPUStart = heap->GetRaw()->GetGPUDescriptorHandleForHeapStart();
	}

	const DescriptorHandle handle(index, type, heapCPUStart, heapGPUStart);
	allocator->Free(handle);
}

DescriptorHeap* DescriptorHeapManager::GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return m_HeapSRV.get();
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return m_HeapSampler.get();
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return m_HeapRenderTarget.get();
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return m_HeapDepthStencil.get();
	default: return nullptr;
	}
}

DescriptorAllocator* DescriptorHeapManager::GetAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return m_AllocatorSRV.get();
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return m_AllocatorSampler.get();
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return m_AllocatorRenderTarget.get();
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return m_AllocatorDepthStencil.get();
	default: return nullptr;
	}
}

// Resets all descriptor heap resources
void DescriptorHeapManager::Shutdown() noexcept
{
	m_HeapSRV.reset();
	m_AllocatorSRV.reset();

	m_HeapSampler.reset();
	m_AllocatorSampler.reset();

	m_HeapDepthStencil.reset();
	m_AllocatorDepthStencil.reset();

	m_HeapRenderTarget.reset();
	m_AllocatorRenderTarget.reset();	
}