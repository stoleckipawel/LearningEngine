// DescriptorAllocator: Free-list based allocator for D3D12 descriptor heaps.
// Provides fast Allocate/Free of individual descriptor slots by index.
// Thread-safe via internal mutex.

#include <stack>
#include <mutex>
#include "DescriptorHeap.h"

// Manages allocation of descriptor indices within a single descriptor heap.
class DescriptorAllocator
{
public:
    // Constructs allocator for an existing heap (does not take ownership).
    explicit DescriptorAllocator(DescriptorHeap* Heap) : m_Heap(Heap) {}

    // Allocates a single descriptor slot.
    // Strategy:
    //   1) Reuse an index from the free-list if available.
    //   2) Otherwise use the next linear index if within heap size.
    // Returns: valid DescriptorHandle on success, invalid on failure.
    DescriptorHandle Allocate();

    // Returns a previously allocated descriptor slot to the free-list.
    // The provided handle must be valid and come from this allocator's heap.
    void Free(const DescriptorHandle& handle) noexcept;    
private:
    DescriptorHeap* m_Heap;           // Heap being managed (not owned)
    std::stack<UINT> m_freeIndices;   // LIFO container of freed indices available for reuse
    UINT m_currentOffset = 0;         // Next unallocated index for linear growth when free-list is empty
    std::mutex m_mutex;               // Guards all Allocate/Free operations for thread safety
};