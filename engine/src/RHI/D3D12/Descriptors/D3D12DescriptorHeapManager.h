#pragma once

#include "D3D12Rhi.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12DescriptorAllocator.h"

// Manages all descriptor heaps required by the engine.
// Encapsulates creation, binding, reset, and cleanup for:
// - CBV/SRV/UAV (shader visible)
// - Sampler (shader visible)
// - Depth Stencil View (DSV)
// - Render Target View (RTV)
// Provides accessors and lifecycle methods.
class D3D12DescriptorHeapManager
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
	void AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type,
	                    D3D12_CPU_DESCRIPTOR_HANDLE& outCPU,
	                    D3D12_GPU_DESCRIPTOR_HANDLE& outGPU);
	// Raw-handle free: reconstructs the owning index from `cpuHandle` and returns
	// the slot to the allocator for the heap mapped by `type`.
	void FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type,
	                D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
	                D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	D3D12DescriptorHandle AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		return GetAllocator(type)->Allocate();
	}
	void FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12DescriptorHandle& handle)
	{
		GetAllocator(type)->Free(handle);
	}

	// Internal mapping helpers used by the generic APIs above.
	// Switch-based mapping selects the correct heap/allocator for the given type.
	D3D12DescriptorHeap* GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept;
	D3D12DescriptorAllocator* GetAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) const noexcept;

  private:
	std::unique_ptr<D3D12DescriptorHeap> m_HeapSRV; // CBV/SRV/UAV heap (shader visible)
	std::unique_ptr<D3D12DescriptorAllocator> m_AllocatorSRV;

	std::unique_ptr<D3D12DescriptorHeap> m_HeapSampler; // Sampler heap (shader visible)
	std::unique_ptr<D3D12DescriptorAllocator> m_AllocatorSampler;

	std::unique_ptr<D3D12DescriptorHeap> m_HeapDepthStencil; // Depth Stencil View heap (non-visible)
	std::unique_ptr<D3D12DescriptorAllocator> m_AllocatorDepthStencil;

	std::unique_ptr<D3D12DescriptorHeap> m_HeapRenderTarget; // Render Target View heap (non-visible)
	std::unique_ptr<D3D12DescriptorAllocator> m_AllocatorRenderTarget;
};

// Global instance for engine-wide access.
extern D3D12DescriptorHeapManager GD3D12DescriptorHeapManager;
