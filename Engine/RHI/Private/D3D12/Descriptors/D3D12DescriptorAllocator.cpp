#include "PCH.h"
#include "D3D12DescriptorAllocator.h"
#include "Log.h"

D3D12DescriptorHandle D3D12DescriptorAllocator::Allocate()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	uint32_t index = ~0u;

	if (!m_freeIndices.empty())
	{
		index = m_freeIndices.back();
		m_freeIndices.pop_back();
	}
	else if (m_currentOffset < m_Heap->GetNumDescriptors())
	{
		index = m_currentOffset++;
	}
	else
	{
		LOG_FATAL("Descriptor heap is full.");
	}

	if (index == ~0u)
	{
		LOG_FATAL("Invalid descriptor index.");
	}

	return m_Heap->GetHandleAt(index);
}

D3D12DescriptorHandle D3D12DescriptorAllocator::AllocateContiguous(uint32_t count)
{
	if (count == 0)
	{
		return D3D12DescriptorHandle{};
	}

	std::lock_guard<std::mutex> lock(m_mutex);

	// Contiguous allocation requires linear growth - cannot use fragmented free-list
	if (m_currentOffset + count > m_Heap->GetNumDescriptors())
	{
		LOG_FATAL("Descriptor heap cannot allocate contiguous block (insufficient space).");
		return D3D12DescriptorHandle{};
	}

	uint32_t startIndex = m_currentOffset;
	m_currentOffset += count;

	return m_Heap->GetHandleAt(startIndex);
}

void D3D12DescriptorAllocator::Free(const D3D12DescriptorHandle& handle) noexcept
{
	if (handle.IsValid())
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_freeIndices.push_back(handle.GetIndex());
	}
}

void D3D12DescriptorAllocator::FreeContiguous(const D3D12DescriptorHandle& firstHandle, uint32_t count) noexcept
{
	if (firstHandle.IsValid() && count > 0)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		uint32_t startIndex = firstHandle.GetIndex();
		for (uint32_t i = 0; i < count; ++i)
		{
			m_freeIndices.push_back(startIndex + i);
		}
	}
}