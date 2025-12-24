
#include "PCH.h"
#include "D3D12Sampler.h"
#include "D3D12Rhi.h"
#include "D3D12DescriptorHeapManager.h"
#include "Log.h"

// Sampler: constructs and creates the D3D12 sampler in the descriptor heap via allocator.
D3D12Sampler::D3D12Sampler()
{
	// Allocate a descriptor slot from the sampler heap. If allocation fails
	// the allocator will have already logged; defensively check here too.
	m_samplerHandle = GD3D12DescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
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
	GD3D12Rhi.GetDevice()->CreateSampler(&samplerDesc, m_samplerHandle.GetCPU());
}

D3D12Sampler::D3D12Sampler(D3D12Sampler&& other) noexcept : m_samplerHandle(other.m_samplerHandle)
{
	// Invalidate source to transfer ownership.
	other.m_samplerHandle = D3D12DescriptorHandle();
}

D3D12Sampler::~D3D12Sampler() noexcept
{
	if (m_samplerHandle.IsValid())
	{
		GD3D12DescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_samplerHandle);
	}
}
