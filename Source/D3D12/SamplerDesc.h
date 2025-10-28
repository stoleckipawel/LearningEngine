#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class SamplerDesc : public D3D12_STATIC_SAMPLER_DESC
{
public:
    SamplerDesc();
    D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor(void);
    D3D12_CPU_DESCRIPTOR_HANDLE Handle;  
    UINT m_DescriptorHandleIndex = 0;
};

