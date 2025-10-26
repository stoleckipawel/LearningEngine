#include "SamplerDesc.h"

SamplerDesc::SamplerDesc()
{
    Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
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
    
    //GRHI.Device->CreateSampler(&desc, Handle);    
}