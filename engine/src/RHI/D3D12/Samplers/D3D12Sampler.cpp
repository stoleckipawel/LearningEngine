#include "PCH.h"
#include "D3D12Sampler.h"

#include "D3D12DescriptorHeapManager.h"
#include "D3D12Rhi.h"
#include "Log.h"

D3D12Sampler::D3D12Sampler(const D3D12_SAMPLER_DESC& samplerDesc)
{
	m_samplerHandle = GD3D12DescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!m_samplerHandle.IsValid())
	{
		LOG_FATAL("Sampler: failed to allocate sampler descriptor.");
		return;
	}

	GD3D12Rhi.GetDevice()->CreateSampler(&samplerDesc, m_samplerHandle.GetCPU());
}

D3D12Sampler::~D3D12Sampler() noexcept
{
	if (m_samplerHandle.IsValid())
	{
		GD3D12DescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_samplerHandle);
	}
}
