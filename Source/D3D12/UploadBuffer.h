#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class FUploadBuffer
{
public:
	static void Upload(ComPointer<ID3D12Resource2>& buffer, void* data, uint32_t dataSize);
};
