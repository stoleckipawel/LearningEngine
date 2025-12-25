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
	// Strategy:
	//   1) Reuse an index from the free-list if available.
	//   2) Otherwise use the next linear index if within heap size.
	// Returns: valid DescriptorHandle on success, invalid on failure.
	D3D12DescriptorHandle Allocate();

	// Returns a previously allocated descriptor slot to the free-list.
	// The provided handle must be valid and come from this allocator's heap.
	void Free(const D3D12DescriptorHandle& handle) noexcept;

  private:
	D3D12DescriptorHeap* m_Heap;  // Heap being managed (not owned)
	// Vector-backed LIFO free-list.
	std::vector<UINT> m_freeIndices;

	// Next unallocated index for linear growth when free-list is empty.
	UINT m_currentOffset = 0;
	std::mutex m_mutex;  // Guards all Allocate/Free operations for thread safety
};