#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class FDescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT FrameIndex, UINT Index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameCPUHandle(UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT FrameIndex, UINT Index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentFrameGPUHandle(UINT Index);
public:
	ComPointer<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
private:

};

