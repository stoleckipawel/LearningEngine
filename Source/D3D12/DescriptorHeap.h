#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class DescriptorHeap
{
public:
	void Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name);

	void InitializeCBVSRVUAV(UINT numCBV, UINT numSRV, UINT numUAV, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR name);
	
	UINT GetCBVOffset(UINT backBufferFrameIndex, UINT index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCBVCPUHandle(UINT index, UINT frameIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetCBVGPUHandle(UINT index, UINT frameIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCBVCPUHandle(UINT index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetCBVGPUHandle(UINT index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index);
private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleInternal(UINT index, UINT frameIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleInternal(UINT index, UINT frameIndex);
public:
	ComPointer<ID3D12DescriptorHeap> Heap;
private:
	D3D12_DESCRIPTOR_HEAP_DESC m_heapDesc = {};
	UINT m_numCBV = 0;
	UINT m_numSRV = 0;
	UINT m_numUAV = 0;
};

