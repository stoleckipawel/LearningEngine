#include "PCH.h"
#include "DescriptorHeap.h"

// General descriptor heap (RTV, DSV, Sampler, etc)
DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name)
{
	m_heapDesc.NumDescriptors = numDescriptors;
	m_heapDesc.Type = type;
	m_heapDesc.Flags = flags;

	// Create descriptor heap
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&m_heapDesc,
		IID_PPV_ARGS(&m_heap)), "DescriptorHeap: Failed To Create Descriptor Heap");
	m_heap->SetName(name);
}

// CBV/SRV/UAV descriptor heap
DescriptorHeap::DescriptorHeap(UINT numCBV, UINT numSRV, UINT numUAV, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name)
{
	m_numCBV = numCBV;
	m_numSRV = numSRV;
	m_numUAV = numUAV;

	// Calculate total descriptors
	UINT numDescriptors = numCBV * NumFramesInFlight + numSRV + numUAV;

	m_heapDesc.NumDescriptors = numDescriptors;
	m_heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	m_heapDesc.Flags = flags;
	// Create descriptor heap
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&m_heapDesc,
		IID_PPV_ARGS(&m_heap)), "DescriptorHeap: Failed To Create Descriptor Heap");
	m_heap->SetName(name);
}

// Move constructor
DescriptorHeap::DescriptorHeap(DescriptorHeap&& other) noexcept
	: m_heap(std::move(other.m_heap)),
	  m_heapDesc(other.m_heapDesc),
	  m_numCBV(other.m_numCBV),
	  m_numSRV(other.m_numSRV),
	  m_numUAV(other.m_numUAV)
{
	other.m_heap = nullptr;
}

// Move assignment
DescriptorHeap& DescriptorHeap::operator=(DescriptorHeap&& other) noexcept
{
	if (this != &other)
	{
		m_heap.Release();
		m_heap = std::move(other.m_heap);
		m_heapDesc = other.m_heapDesc;
		m_numCBV = other.m_numCBV;
		m_numSRV = other.m_numSRV;
		m_numUAV = other.m_numUAV;
		other.m_heap = nullptr;
	}
	return *this;
}

// Destructor
DescriptorHeap::~DescriptorHeap() noexcept
{
	m_heap.Release();
}

// Get offset for descriptor type
UINT DescriptorHeap::GetTypeOffset(DescriptorType type) const
{
	switch (type)
	{
	case DescriptorType::CBV:
		return 0;
	case DescriptorType::SRV:
		// SRV starts after CBVs
		return m_numCBV * NumFramesInFlight;
	case DescriptorType::UAV:
		// UAV starts after CBVs and SRVs
		return m_numCBV * NumFramesInFlight + m_numSRV;
	case DescriptorType::Other:
	default:
		// Other types use offset 0
		return 0;
	}
}

// Get CPU descriptor handle
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(UINT indexInType, DescriptorType type)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(m_heapDesc.Type);
	// Offset by type and index
	handle.ptr += descriptorSize * (indexInType + GetTypeOffset(type));
	return handle;
}

// Get GPU descriptor handle
D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(UINT indexInType, DescriptorType type)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_heap->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(m_heapDesc.Type);
	// Offset by type and index
	handle.ptr += descriptorSize * (indexInType + GetTypeOffset(type));
	return handle;
}

