#pragma once

#include "../Vendor/Windows/WinInclude.h"

class FSamplerDesc : public D3D12_STATIC_SAMPLER_DESC
{
public:
    FSamplerDesc();

    // Allocate new descriptor as needed; return handle to existing descriptor when possible
    D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor(void);

    // Create descriptor in place (no deduplication).  Handle must be preallocated
    void CreateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE Handle);
};

