#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"

// Sampler class manages a D3D12 sampler and its descriptor handles
class Sampler
{
public:
    // Constructs a sampler and creates it in the descriptor heap
    Sampler(UINT DescriptorHandleIndex);
    // Destructor releases resources 
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    // Returns the GPU descriptor handle for this sampler
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

    // Returns the CPU descriptor handle for this sampler
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
private:
    UINT m_DescriptorHandleIndex = 0;  // Index in the sampler descriptor heap
    D3D12_SAMPLER_DESC m_desc = {};    // Sampler description
};

