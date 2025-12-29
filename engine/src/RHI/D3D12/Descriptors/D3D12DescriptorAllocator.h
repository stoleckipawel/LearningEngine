#pragma once

// D3D12DescriptorAllocator: Free-list based allocator for D3D12 descriptor heaps.
// Provides fast Allocate/Free of individual descriptor slots by index.
// Thread-safe via internal mutex.

#include <vector>
#include <mutex>
#include "D3D12DescriptorHeap.h"

// Manages allocation of descriptor indices within a single descriptor heap.
class D3D12DescriptorAllocator
{
  public:
	// Constructs allocator for an existing heap (does not take ownership).
	explicit D3D12DescriptorAllocator(D3D12DescriptorHeap* Heap) : m_Heap(Heap) {}

	// Allocates a single descriptor slot.
	D3D12DescriptorHandle Allocate();

	// Allocates a contiguous block of descriptor slots.
	// Used for descriptor tables that require sequential descriptors.
	// Returns handle to first descriptor; subsequent slots are offset by descriptor size.
	D3D12DescriptorHandle AllocateContiguous(uint32_t count);

	// Returns a previously allocated descriptor slot to the free-list.
	void Free(const D3D12DescriptorHandle& handle) noexcept;

	// Returns a contiguous block to the free-list.
	void FreeContiguous(const D3D12DescriptorHandle& firstHandle, uint32_t count) noexcept;

  private:
	D3D12DescriptorHeap* m_Heap;
	std::vector<UINT> m_freeIndices;
	UINT m_currentOffset = 0;
	std::mutex m_mutex;
};