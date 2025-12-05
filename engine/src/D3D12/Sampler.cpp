
#include "Core/PCH.h"
#include "D3D12/Sampler.h"
#include "D3D12/RHI.h"
#include "D3D12/DescriptorHeapManager.h"

// Constructs a Sampler and creates the D3D12 sampler in the descriptor heap
Sampler::Sampler(UINT DescriptorHandleIndex)
    : m_DescriptorHandleIndex(DescriptorHandleIndex)
{
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    // Create the sampler in the descriptor heap
    GRHI.Device->CreateSampler(&samplerDesc, GetCPUHandle());
}

// Returns the GPU descriptor handle for this sampler
D3D12_GPU_DESCRIPTOR_HANDLE Sampler::GetGPUHandle() const
{
    return GDescriptorHeapManager.GetSamplerHeap().GetGPUHandle(m_DescriptorHandleIndex);
}

// Returns the CPU descriptor handle for this sampler
D3D12_CPU_DESCRIPTOR_HANDLE Sampler::GetCPUHandle() const
{
    return GDescriptorHeapManager.GetSamplerHeap().GetCPUHandle(m_DescriptorHandleIndex);
}
