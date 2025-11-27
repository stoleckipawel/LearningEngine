#pragma once

#include "RHI.h"

enum class DescriptorType
{
	CBV,
	SRV,
	UAV,
	Other // Covers RTV, Sampler, DSV, etc
};

// Manages a single D3D12 descriptor heap.
class DescriptorHeap
{
public:
	// No copy allowed, strict ownership
	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap& operator=(const DescriptorHeap&) = delete;

	// Create general descriptor heap (RTV, DSV, Sampler, etc)
	explicit DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name);
	// Create CBV/SRV/UAV descriptor heap
	explicit DescriptorHeap(UINT numCBV, UINT numSRV, UINT numUAV, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name);

	// Move constructor and assignment
	DescriptorHeap(DescriptorHeap&& other) noexcept;
	DescriptorHeap& operator=(DescriptorHeap&& other) noexcept;

	// Destructor releases the descriptor heap
	~DescriptorHeap() noexcept;

	// Get CPU handle for descriptor type and index
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT indexInType, DescriptorType type = DescriptorType::Other);
	// Get GPU handle for descriptor type and index
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT indexInType, DescriptorType type = DescriptorType::Other);

	// Get raw ID3D12DescriptorHeap pointer
	ID3D12DescriptorHeap* GetRaw() noexcept { return m_heap.Get(); }
private:
	// Get offset for descriptor type in heap
	UINT GetTypeOffset(DescriptorType type) const;
	D3D12_DESCRIPTOR_HEAP_DESC m_heapDesc = {};
	UINT m_numCBV = 0;
	UINT m_numSRV = 0;
	UINT m_numUAV = 0;
	ComPointer<ID3D12DescriptorHeap> m_heap;
};

