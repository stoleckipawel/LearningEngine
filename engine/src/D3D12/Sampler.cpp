
#include "PCH.h"
#include "DX12/Sampler.h"
#include "DX12/RHI.h"
#include "DX12/DescriptorHeapManager.h"

// Constructs a Sampler and creates the D3D12 sampler in the descriptor heap
Sampler::Sampler(UINT DescriptorHandleIndex)
    : m_DescriptorHandleIndex(DescriptorHandleIndex)
{
    // Set up default sampler description
    m_desc.Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    m_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_desc.MipLODBias = 0.0f;
    m_desc.MaxAnisotropy = 0;
    m_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    m_desc.MinLOD = 0.0f;
    m_desc.MaxLOD = D3D12_FLOAT32_MAX;

    // Create the sampler in the descriptor heap
    GRHI.Device->CreateSampler(&m_desc, GetCPUHandle());
}

Sampler::~Sampler()
{

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
