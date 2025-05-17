#include "D3D12DescriptorHeap.h"

void D3D12DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, LPCWSTR Name)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = NumDescriptors;
	heapDesc.Type = Type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;

	
	D3D12Context::Get().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&heap));

	heap->SetName(Name);
}
