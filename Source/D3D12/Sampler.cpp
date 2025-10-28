#include "Sampler.h"

Sampler::Sampler(UINT DescriptorHandleIndex)
    : m_DescriptorHandleIndex(DescriptorHandleIndex)
{
    m_desc.Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    m_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_desc.MipLODBias = 0.0f;
    m_desc.MaxAnisotropy = 0;
    m_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    m_desc.MinLOD = 0.0f;
    m_desc.MaxLOD = D3D12_FLOAT32_MAX;
    
    GRHI.Device->CreateSampler(&m_desc, GetCPUHandle());    
}

D3D12_GPU_DESCRIPTOR_HANDLE Sampler::GetGPUHandle()
{
    return GDescriptorHeapManager.GetSamplerHeap().GetCurrentFrameGPUHandle(m_DescriptorHandleIndex);
}   

D3D12_CPU_DESCRIPTOR_HANDLE Sampler::GetCPUHandle()
{
    return GDescriptorHeapManager.GetSamplerHeap().GetCurrentFrameCPUHandle(m_DescriptorHandleIndex);
}   
