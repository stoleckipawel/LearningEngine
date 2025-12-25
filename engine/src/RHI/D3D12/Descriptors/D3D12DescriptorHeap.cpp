#include "PCH.h"
#include "D3D12DescriptorHeap.h"
#include "DebugUtils.h"

// General descriptor heap (RTV, DSV, Sampler, SRV/CBV/UAV).
D3D12DescriptorHeap::D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name)
{
	m_desc.Type = type;
	m_desc.Flags = flags;
	m_desc.NumDescriptors = GetNumDescriptors();

	// Create descriptor heap
	CHECK(GD3D12Rhi.GetDevice()->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(m_heap.ReleaseAndGetAddressOf())));
	DebugUtils::SetDebugName(m_heap, name);
}

D3D12DescriptorHeap::~D3D12DescriptorHeap() noexcept
{
	m_heap.Reset();
}

D3D12DescriptorHandle D3D12DescriptorHeap::GetHandleAt(UINT index) const
{
	if (index >= m_desc.NumDescriptors)
	{
		// Guard: index must be within heap descriptor count
		LOG_FATAL("Index out of range");
	}

	// For shader-visible heaps, provide the GPU start handle; otherwise leave GPU handle zeroed.
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {0};
	if (m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		gpuHandle = m_heap->GetGPUDescriptorHandleForHeapStart();
	}

	return D3D12DescriptorHandle(index, m_desc.Type, m_heap->GetCPUDescriptorHandleForHeapStart(), gpuHandle);
}

UINT D3D12DescriptorHeap::GetNumDescriptors() const
{
	return m_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE
	                                                         : D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2;
}