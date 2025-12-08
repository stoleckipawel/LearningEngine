#include "PCH.h"
#include "DescriptorAllocator.h"

// Allocate a descriptor slot using a free-list with linear fallback.
// Strategy:
//  - Prefer reusing freed indices (LIFO) to improve locality and reduce fragmentation.
//  - Otherwise, grow linearly while within the heap's current size.
//  - If the heap is full and cannot grow, report a fatal error and return an invalid handle.
// Returns: DescriptorHandle containing CPU/GPU pointers (GPU only if shader-visible), or invalid on failure.
DescriptorHandle DescriptorAllocator::Allocate()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Start with invalid index sentinel; updated on success.
    uint32_t index_to_use = ~0u;

    if (!m_freeIndices.empty())
    {
        // 1) Prefer reusing a previously freed descriptor index.
        //    Rationale:
        //    - Keeps descriptor usage dense, improving cache locality when the CPU writes descriptors.
        //    - Avoids growing the linear tail unnecessarily, which reduces risk of hitting heap capacity.
        //    - LIFO is chosen for simplicity and good locality (recently freed often reused soon).
        index_to_use = m_freeIndices.top();
        m_freeIndices.pop();
    }
    else if (m_currentOffset < m_Heap->GetNumDescriptors())
    {
        // 2) Fall back to linear growth from the beginning of the heap.
        //    Rationale:
        //    - When the free-list is empty, we allocate the next sequential index.
        //    - This yields contiguous descriptor ranges, which is efficient for handle math
        //      and minimizes fragmentation in shader-visible heaps.
        //    - We stop at the heap's current size; resizing (if supported) would extend this bound.
        index_to_use = m_currentOffset;
        m_currentOffset++;
    }
    else
    {
        LogMessage("DescriptorAllocator: Heap is full and cannot grow (max capacity reached).", ELogType::Fatal);
    }

    // Convert the index to a typed DescriptorHandle via the heap.
    return m_Heap->GetHandleAt(index_to_use);
}

// Free a previously allocated descriptor slot by pushing its index onto the free-list.
// The handle must be valid and originate from this allocator's heap.
void DescriptorAllocator::Free(const DescriptorHandle& handle) noexcept
{
    if (handle.IsValid())
    {
        // Only the index is needed to return the slot to the free-list.
        std::lock_guard<std::mutex> lock(m_mutex);
        m_freeIndices.push(handle.GetIndex());
    }
}