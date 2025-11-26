#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"

// Sampler class manages a D3D12 sampler and its descriptor handles
class Sampler
{
public:
    Sampler() = default;

    // Constructs a sampler and creates it in the descriptor heap
    Sampler(UINT DescriptorHandleIndex);

    // Returns the GPU descriptor handle for this sampler
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

    // Returns the CPU descriptor handle for this sampler
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
private:
    UINT m_DescriptorHandleIndex = 0;  
    D3D12_SAMPLER_DESC m_desc = {}; 
};

