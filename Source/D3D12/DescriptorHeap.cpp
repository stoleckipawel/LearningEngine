#include "DescriptorHeap.h"

void FDescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name)
{
	heapDesc.NumDescriptors = NumDescriptors;
	heapDesc.Type = Type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;
	
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&heap)), "DescriptorHeap: Failed To Create Descriptor Heap");

	heap->SetName(Name);
}

D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetCPUHandle(UINT FrameIndex, UINT Index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	handle.ptr = descriptorSize * (FrameIndex + Index) + handle.ptr;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetCurrentFrameCPUHandle(UINT Index)
{
	return GetCPUHandle(GSwapChain.GetCurrentBackBufferIndex(), Index);
}


D3D12_GPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetGPUHandle(UINT FrameIndex, UINT Index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	handle.ptr = descriptorSize * (FrameIndex + Index) + handle.ptr;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetCurrentFrameGPUHandle(UINT Index)
{
	return GetGPUHandle(GSwapChain.GetCurrentBackBufferIndex(), Index);
}