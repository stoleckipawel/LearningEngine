#pragma once
#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"

class D3D12DescriptorHeap
{
public:
	D3D12DescriptorHeap() = default;
	D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors);
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors);
public:
	ComPointer<ID3D12DescriptorHeap> heap;
private:

};

