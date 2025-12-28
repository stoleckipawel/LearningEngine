#pragma once

#include "D3D12DescriptorHandle.h"
#include <d3d12.h>

// Sampler: lightweight RAII wrapper for a D3D12 sampler descriptor.
// Ownership:
//  - Allocates a descriptor slot from the sampler heap on construction.
//  - Frees the slot on destruction.
//
// Copy and move are disabled: the sampler owns a unique heap allocation.
class D3D12Sampler
{
  public:
	explicit D3D12Sampler(const D3D12_SAMPLER_DESC& samplerDesc);
	~D3D12Sampler() noexcept;

	D3D12Sampler(const D3D12Sampler&) = delete;
	D3D12Sampler& operator=(const D3D12Sampler&) = delete;
	D3D12Sampler(D3D12Sampler&&) = delete;
	D3D12Sampler& operator=(D3D12Sampler&&) = delete;

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_samplerHandle.GetGPU(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_samplerHandle.GetCPU(); }
	bool IsValid() const noexcept { return m_samplerHandle.IsValid(); }

  private:
	D3D12DescriptorHandle m_samplerHandle;
};
