#include "SamplerDesc.h"

FSamplerDesc::FSamplerDesc()
{
    // These defaults match the default values for HLSL-defined root
    // signature static samplers.  So not overriding them here means
    // you can safely not define them in HLSL.
    Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    MipLODBias = 0.0f;
    MaxAnisotropy = 0;
    ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    MinLOD = 0.0f;
    MaxLOD = D3D12_FLOAT32_MAX;
    ShaderRegister = 0;
    RegisterSpace = 0;
    ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;    
}

void FSamplerDesc::CreateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
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

D3D12_CPU_DESCRIPTOR_HANDLE FSamplerDesc::CreateDescriptor(void)
{
    //ASSERT(Handle.ptr != 0 && Handle.ptr != -1);
    //GRHI.Device->CreateSampler(this, Handle);
	return D3D12_CPU_DESCRIPTOR_HANDLE(); // Placeholder, should return a valid handle
}