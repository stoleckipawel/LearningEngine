#pragma once
#include "D3D12/DescriptorHandle.h"

// Sampler class manages a D3D12 sampler and its descriptor handles
class Sampler
{
public:
    // Constructs a sampler and creates it in the descriptor heap via allocator
    Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    // Returns the GPU descriptor handle for this sampler
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_samplerHandle.GetGPU(); }

    // Returns the CPU descriptor handle for this sampler
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_samplerHandle.GetCPU(); }
    ~Sampler();
private:
    DescriptorHandle m_samplerHandle;  // Allocated sampler descriptor handle
};

