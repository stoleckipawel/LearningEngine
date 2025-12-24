#pragma once

#include "DescriptorHandle.h"
#include <d3d12.h>

// Sampler: lightweight RAII wrapper for a D3D12 sampler descriptor.
// Ownership:
//  - Allocates a descriptor slot from the sampler heap on construction.
//  - Frees the slot on destruction.
// Move semantics transfer descriptor ownership; copy is disabled to avoid
// double-freeing a descriptor index.
class Sampler
{
public:
    // Default constructs and creates a sampler descriptor in the sampler heap.
    // If allocation fails an error will be reported (engine fatal path).
    Sampler();

    Sampler(const Sampler&) = delete;
    Sampler(Sampler&& other) noexcept;

    // Accessors are noexcept and const-correct. Return by-value is cheap
    // (handles are small PODs).
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_samplerHandle.GetGPU(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_samplerHandle.GetCPU(); }
    bool IsValid() const noexcept { return m_samplerHandle.IsValid(); }

    ~Sampler() noexcept;

private:
    DescriptorHandle m_samplerHandle;  // Allocated sampler descriptor handle
};

