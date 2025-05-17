#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12Context.h"

class D3D12UploadBuffer
{
public:
	static void Upload(ComPointer<ID3D12Resource2>& buffer, void* data, uint32_t dataSize);
};
