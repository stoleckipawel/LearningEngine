#include "DescriptorHeap.h"

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptorsPerFrame, bool bFrameBuffered, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name)
{
	this->NumDescriptorsPerFrame = NumDescriptorsPerFrame;
	this->bFrameBuffered = bFrameBuffered;
	this->NumDescriptors = NumDescriptorsPerFrame * (bFrameBuffered ? NumFramesInFlight : 1);

	heapDesc.NumDescriptors = NumDescriptors;
	heapDesc.Type = Type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;
	
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&heap)), "DescriptorHeap: Failed To Create Descriptor Heap");

	heap->SetName(Name);
}

UINT DescriptorHeap::GetHandleOffset(UINT BackBufferFrameIndex, UINT Index)
{
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	UINT groupOffset = bFrameBuffered ? BackBufferFrameIndex * NumDescriptorsPerFrame : 0;
	return descriptorSize * (groupOffset + Index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(UINT BackBufferFrameIndex, UINT Index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr = GetHandleOffset(BackBufferFrameIndex, Index) + handle.ptr;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCurrentFrameCPUHandle(UINT Index)
{
	return GetCPUHandle(GSwapChain.GetBackBufferIndex(), Index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(UINT BackBufferFrameIndex, UINT Index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr = GetHandleOffset(BackBufferFrameIndex, Index) + handle.ptr;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCurrentFrameGPUHandle(UINT Index)
{
	return GetGPUHandle(GSwapChain.GetBackBufferIndex(), Index);
}