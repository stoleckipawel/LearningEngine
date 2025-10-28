#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"

class Sampler
{
public:
    Sampler() = default;
    Sampler(UINT DescriptorHandleIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();  
private:     
    UINT m_DescriptorHandleIndex = 0;   
    D3D12_SAMPLER_DESC m_desc = {};
};

