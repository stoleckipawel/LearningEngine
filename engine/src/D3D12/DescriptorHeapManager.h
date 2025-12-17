#pragma once

#include "RHI.h"
#include "DescriptorHeap.h"
#include "DescriptorAllocator.h"

// Manages all descriptor heaps required by the engine.
// Encapsulates creation, binding, reset, and cleanup for:
// - CBV/SRV/UAV (shader visible)
// - Sampler (shader visible)
// - Depth Stencil View (DSV)
// - Render Target View (RTV)
// Provides accessors and lifecycle methods.
class DescriptorHeapManager
{
public:
	// Initializes all descriptor heaps. Safe to call once at startup.
	void Initialize();

	// Binds shader-visible heaps (CBV/SRV/UAV and Sampler) to the current command list.
	void SetShaderVisibleHeaps() const;

	// Resets all descriptor heap resources.
	void Shutdown() noexcept;

	// Raw-handle variant: allocates from the heap mapped by `type` and
	// returns materialized CPU/GPU handles. Intended for libraries (e.g., ImGui)
	// which operate on raw D3D12 handles rather than engine `DescriptorHandle`s.
	void AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE& outCPU, D3D12_GPU_DESCRIPTOR_HANDLE& outGPU);
	// Raw-handle free: reconstructs the owning index from `cpuHandle` and returns
	// the slot to the allocator for the heap mapped by `type`.
	void FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	DescriptorHandle AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type) { return GetAllocator(type)->Allocate(); }
	void FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, const DescriptorHandle& handle) { GetAllocator(type)->Free(handle); }

	// Internal mapping helpers used by the generic APIs above.
	// Switch-based mapping selects the correct heap/allocator for the given type.
	DescriptorHeap* GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
	DescriptorAllocator* GetAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
private:
	std::unique_ptr<DescriptorHeap> m_HeapSRV;        // CBV/SRV/UAV heap (shader visible)
	std::unique_ptr<DescriptorAllocator> m_AllocatorSRV; 

	std::unique_ptr<DescriptorHeap> m_HeapSampler;    // Sampler heap (shader visible)
	std::unique_ptr<DescriptorAllocator> m_AllocatorSampler; 

	std::unique_ptr<DescriptorHeap> m_HeapDepthStencil; // Depth Stencil View heap (non-visible)
	std::unique_ptr<DescriptorAllocator> m_AllocatorDepthStencil; 

	std::unique_ptr<DescriptorHeap> m_HeapRenderTarget; // Render Target View heap (non-visible)
	std::unique_ptr<DescriptorAllocator> m_AllocatorRenderTarget; 
};

// Global instance for engine-wide access.
extern DescriptorHeapManager GDescriptorHeapManager;






