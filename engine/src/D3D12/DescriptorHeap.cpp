#include "PCH.h"
#include "D3D12/DescriptorHeap.h"


// General descriptor heap (RTV, DSV, Sampler, SRV/CBV/UAV).
DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name)
{
	m_desc.Type = type;
	m_desc.Flags = flags;
	m_desc.NumDescriptors = GetNumDescriptors();

	// Create descriptor heap
	ThrowIfFailed(GRHI.GetDevice()->CreateDescriptorHeap(
		&m_desc,
		IID_PPV_ARGS(m_heap.ReleaseAndGetAddressOf())), "DescriptorHeap: Failed To Create Descriptor Heap");
	m_heap->SetName(name);
}

DescriptorHeap::~DescriptorHeap() noexcept
{
	m_heap.Reset();
}

DescriptorHandle DescriptorHeap::GetHandleAt(UINT index) const
{
	if (index >= m_desc.NumDescriptors)
	{
		// Guard: index must be within heap descriptor count
		LogMessage("DescriptorHeap: index out of range", ELogType::Fatal);
		// Return an invalid handle to avoid undefined pointer math
		return DescriptorHandle(~0u, m_desc.Type, { 0 }, { 0 });
	}

	// Materialize a typed handle using heap start CPU/GPU handles.
	// The handle computes final CPU/GPU pointer by adding index * increment size.
	// Only call GetGPUDescriptorHandleForHeapStart for shader-visible heaps.
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = { 0 };
	if (m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		gpuHandle = m_heap->GetGPUDescriptorHandleForHeapStart();
	}

	return DescriptorHandle(
		index,
		m_desc.Type,
		m_heap->GetCPUDescriptorHandleForHeapStart(),
		gpuHandle);
}

UINT DescriptorHeap::GetNumDescriptors() const
{
	return m_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
		? D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE
		: D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2;
}