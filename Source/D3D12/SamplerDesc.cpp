#include "SamplerDesc.h"

SamplerDesc::SamplerDesc()
{
    // These defaults match the default values for HLSL-defined root
    // signature static samplers.  So not overriding them here means
    // you can safely not define them in HLSL.
    Filter = D3D12_FILTER_ANISOTROPIC;
    AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    MipLODBias = 0.0f;
    MaxAnisotropy = 16;
    ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    BorderColor[0] = 1.0f;
    BorderColor[1] = 1.0f;
    BorderColor[2] = 1.0f;
    BorderColor[3] = 1.0f;
    MinLOD = 0.0f;
    MaxLOD = D3D12_FLOAT32_MAX;
}

void SamplerDesc::SetBorderColor(const XMFLOAT4& Border)
{
    BorderColor[0] = Border.x;
    BorderColor[1] = Border.y;
    BorderColor[2] = Border.z;
    BorderColor[3] = Border.w;
}

void SamplerDesc::SetTextureAddressMode(D3D12_TEXTURE_ADDRESS_MODE AddressMode)
{
    AddressU = AddressMode;
    AddressV = AddressMode;
    AddressW = AddressMode;
}
void SamplerDesc::CreateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
{
    //size_t hashValue = Utility::HashState(this);
    //auto iter = s_SamplerCache.find(hashValue);
    //if (iter != s_SamplerCache.end())
    //{
        //return iter->second;
    //}

    //D3D12_CPU_DESCRIPTOR_HANDLE Handle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    //GRHI.Device->CreateSampler(this, Handle);
    //return Handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE SamplerDesc::CreateDescriptor(void)
{
    //ASSERT(Handle.ptr != 0 && Handle.ptr != -1);
    //GRHI.Device->CreateSampler(this, Handle);
	return D3D12_CPU_DESCRIPTOR_HANDLE(); // Placeholder, should return a valid handle
}