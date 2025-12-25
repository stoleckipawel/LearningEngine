#pragma once

// D3D12 descriptor heap wrapper.
// Encapsulates creation and basic handle materialization for a single heap.
// Ownership: non-copyable, non-movable RAII around ID3D12DescriptorHeap.
#include "D3D12Rhi.h"
#include "D3D12DescriptorHandle.h"

using Microsoft::WRL::ComPtr;

// Manages a single D3D12 descriptor heap.
class D3D12DescriptorHeap
{
  public:
	// Construct a heap with the given type and flags; name is applied to the COM object.
	explicit D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name);

	D3D12DescriptorHeap(const D3D12DescriptorHeap&) = delete;
	D3D12DescriptorHeap& operator=(const D3D12DescriptorHeap&) = delete;
	D3D12DescriptorHeap(D3D12DescriptorHeap&&) = delete;
	D3D12DescriptorHeap& operator=(D3D12DescriptorHeap&&) = delete;

	// Releases underlying heap.
	~D3D12DescriptorHeap() noexcept;

	// Descriptor count policy.
	// - Sampler: D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE.
	// - Shader-visible others: D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2.
	// - Non-visible (RTV/DSV): currently same as shader-visible (configurable later).
	UINT GetNumDescriptors() const;

	// Raw pointer access for D3D12 binding.
	ID3D12DescriptorHeap* GetRaw() const noexcept { return m_heap.Get(); }

	// Materialize a typed descriptor handle at the given index.
	// Performs bounds check and returns an invalid handle on error.
	D3D12DescriptorHandle GetHandleAt(UINT index) const;

  private:
	D3D12_DESCRIPTOR_HEAP_DESC m_desc = {};
	ComPtr<ID3D12DescriptorHeap> m_heap;
};
