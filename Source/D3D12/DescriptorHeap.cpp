#include "DescriptorHeap.h"

void FDescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptorsPerFrame, bool bFrameBuffered, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name)
{
	this->NumDescriptorsPerFrame = NumDescriptorsPerFrame;
	this->bFrameBuffered = bFrameBuffered;
	this->NumDescriptors = NumDescriptorsPerFrame * (bFrameBuffered ? BufferingCount : 1);

	heapDesc.NumDescriptors = NumDescriptors;
	heapDesc.Type = Type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;
	
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&heap)), "DescriptorHeap: Failed To Create Descriptor Heap");

	heap->SetName(Name);
}

UINT FDescriptorHeap::GetHandleOffset(UINT FrameIndex, UINT Index)
{
	UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	UINT groupOffset = bFrameBuffered ? FrameIndex * NumDescriptorsPerFrame : 0;
	return descriptorSize * (groupOffset + Index);
}

D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetCPUHandle(UINT FrameIndex, UINT Index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr = GetHandleOffset(FrameIndex, Index) + handle.ptr;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetCurrentFrameCPUHandle(UINT Index)
{
	return GetCPUHandle(GSwapChain.GetCurrentBackBufferIndex(), Index);
}

D3D12_GPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetGPUHandle(UINT FrameIndex, UINT Index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr = GetHandleOffset(FrameIndex, Index) + handle.ptr;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE FDescriptorHeap::GetCurrentFrameGPUHandle(UINT Index)
{
	return GetGPUHandle(GSwapChain.GetCurrentBackBufferIndex(), Index);
}