#pragma once

#include "../Vendor/Windows/WinInclude.h"

class SamplerDesc : public D3D12_SAMPLER_DESC
{
    SamplerDesc();
    void SetTextureAddressMode(D3D12_TEXTURE_ADDRESS_MODE AddressMode);
    void SetBorderColor(const XMFLOAT4& Border);

    // Allocate new descriptor as needed; return handle to existing descriptor when possible
    D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor(void);

    // Create descriptor in place (no deduplication).  Handle must be preallocated
    void CreateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE Handle);
};

