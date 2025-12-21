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
        // Reuse the most-recently freed index for locality.
        index_to_use = m_freeIndices.back();
        m_freeIndices.pop_back();
    }
    else if (m_currentOffset < m_Heap->GetNumDescriptors())
    {
        index_to_use = m_currentOffset++;
    }
    else
    {
        // Heap exhausted: log and return an explicitly invalid handle.
        LogMessage("Heap is full and cannot grow (max capacity reached).", ELogType::Fatal);
        return DescriptorHandle();
    }

    // Guard against invalid index (defensive programming).
    if (index_to_use == ~0u)
    {
        return DescriptorHandle();
    }

    // Materialize a typed DescriptorHandle using the heap.
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
        m_freeIndices.push_back(handle.GetIndex());
    }
}