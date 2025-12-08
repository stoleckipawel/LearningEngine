#pragma once

// D3D12 descriptor heap wrapper.
// Encapsulates creation and basic handle materialization for a single heap.
// Ownership: non-copyable, non-movable RAII around ID3D12DescriptorHeap.
#include "RHI.h"
#include "DescriptorHandle.h" 

// Manages a single D3D12 descriptor heap.
class DescriptorHeap
{
public:
	// Construct a heap with the given type and flags; name is applied to the COM object.
	explicit DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name);

	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap& operator=(const DescriptorHeap&) = delete;
	DescriptorHeap(DescriptorHeap&&) = delete;
	DescriptorHeap& operator=(DescriptorHeap&&) = delete;

	// Releases underlying heap.
	~DescriptorHeap() noexcept;

	// Descriptor count policy.
	// - Sampler: D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE.
	// - Shader-visible others: D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2.
	// - Non-visible (RTV/DSV): currently same as shader-visible (configurable later).
	UINT GetNumDescriptors() const;

	// Raw pointer access for D3D12 binding.
	ID3D12DescriptorHeap* GetRaw() noexcept { return m_heap.Get(); }

	// Materialize a typed descriptor handle at the given index.
	// Performs bounds check and returns an invalid handle on error.
	DescriptorHandle GetHandleAt(UINT index) const;

private:
	D3D12_DESCRIPTOR_HEAP_DESC m_desc = {};
	ComPtr<ID3D12DescriptorHeap> m_heap;
};

