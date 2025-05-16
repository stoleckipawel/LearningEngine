#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"

class D3D12DescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, LPCWSTR Name);
public:
	ComPointer<ID3D12DescriptorHeap> heap;
private:

};

