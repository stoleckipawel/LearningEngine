#include "DescriptorHeap.h"

void FDescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = NumDescriptors;
	heapDesc.Type = Type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;

	
	ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&heap)), "DescriptorHeap: Failed To Create Descriptor Heap");

	heap->SetName(Name);
}
