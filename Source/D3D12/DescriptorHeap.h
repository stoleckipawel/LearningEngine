#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class FDescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name);
public:
	ComPointer<ID3D12DescriptorHeap> heap;
private:

};

