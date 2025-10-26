#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class DescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptorsPerFrame, bool bFrameBuffered, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT FrameIndex, UINT Index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameCPUHandle(UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT FrameIndex, UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentFrameGPUHandle(UINT Index);
	UINT GetHandleOffset(UINT FrameIndex, UINT Index);
public:
	ComPointer<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
private:
	UINT NumDescriptorsPerFrame = 0;
	UINT NumDescriptors = 0;
	bool bFrameBuffered = false;
};

