#include "DescriptorHeap.h"

void DescriptorHeap::Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name)
{
	m_heapDesc.NumDescriptors = numDescriptors;
	m_heapDesc.Type = type;
	m_heapDesc.Flags = flags;
	
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&m_heapDesc,
		IID_PPV_ARGS(&Heap)), "DescriptorHeap: Failed To Create Descriptor Heap");

	Heap->SetName(name);
}

void DescriptorHeap::InitializeCBVSRVUAV(UINT numCBV, UINT numSRV, UINT numUAV, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name)
{
	m_numCBV = numCBV;
	m_numSRV = numSRV;	
	m_numUAV = numUAV;
	UINT numDescriptors = numCBV * NumFramesInFlight + numSRV + numUAV;
	Initialize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptors, flags, name);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandleInternal(UINT index, UINT frameIndex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = Heap->GetCPUDescriptorHandleForHeapStart();
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(m_heapDesc.Type);
	handle.ptr += descriptorSize * (frameIndex * m_numCBV + index);
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandleInternal(UINT index, UINT frameIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = Heap->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(m_heapDesc.Type);
	handle.ptr += descriptorSize * (frameIndex * m_numCBV + index);
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCBVCPUHandle(UINT index, UINT frameIndex)
{
	return GetCPUHandleInternal(index, frameIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCBVGPUHandle(UINT index, UINT frameIndex)
{
	return GetGPUHandleInternal(index, frameIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCBVCPUHandle(UINT index)
{
	return GetCBVCPUHandle(index, GSwapChain.GetBackBufferIndex());
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCBVGPUHandle(UINT index)
{
	return GetCBVGPUHandle(index, GSwapChain.GetBackBufferIndex());
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(UINT index)
{
	return GetGPUHandleInternal(index, NumFramesInFlight);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(UINT index)
{
	return GetCPUHandleInternal(index, NumFramesInFlight);
}