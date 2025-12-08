
#include "PCH.h"
#include "D3D12/Sampler.h"
#include "D3D12/RHI.h"
#include "D3D12/DescriptorHeapManager.h"

// Sampler: constructs and creates the D3D12 sampler in the descriptor heap via allocator.
Sampler::Sampler()
    : m_samplerHandle(GDescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER))
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

    // Create the sampler in the descriptor heap.
    GRHI.GetDevice()->CreateSampler(&samplerDesc, GetCPUHandle());
}

Sampler::~Sampler()
{
    if (m_samplerHandle.IsValid())
    {
        GDescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_samplerHandle);
    }
}
