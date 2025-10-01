#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class FUploadBuffer
{
public:
	static ComPointer<ID3D12Resource2> Upload(void* data, uint32_t dataSize);
};
