
#include "PCH.h"
#include "Sampler.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"
#include "Log.h"

// Sampler: constructs and creates the D3D12 sampler in the descriptor heap via allocator.
Sampler::Sampler()
{
    // Allocate a descriptor slot from the sampler heap. If allocation fails
    // the allocator will have already logged; defensively check here too.
    m_samplerHandle = GDescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    if (!m_samplerHandle.IsValid())
    {
        LOG_FATAL("Sampler: failed to allocate sampler descriptor.");
        return;
    }

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

    // Write sampler into the descriptor heap at the allocated CPU handle.
    GRHI.GetDevice()->CreateSampler(&samplerDesc, m_samplerHandle.GetCPU());
}

Sampler::Sampler(Sampler&& other) noexcept
    : m_samplerHandle(other.m_samplerHandle)
{
    // Invalidate source to transfer ownership.
    other.m_samplerHandle = DescriptorHandle();
}

Sampler::~Sampler() noexcept
{
    if (m_samplerHandle.IsValid())
    {
        GDescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_samplerHandle);
    }
}
