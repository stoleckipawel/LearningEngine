#include "RootSignature.h"

void RootSignature::Create()
{
    CD3DX12_DESCRIPTOR_RANGE cbvVertexRange;
    cbvVertexRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, BufferingCount, 0); //b0 

    CD3DX12_DESCRIPTOR_RANGE cbvPixelRange;
    cbvPixelRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, BufferingCount, 0); //b0 

    CD3DX12_DESCRIPTOR_RANGE textureRange;
    textureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); //t0 

    CD3DX12_DESCRIPTOR_RANGE samplerRange;
    samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC); //s0 

    CD3DX12_ROOT_PARAMETER rootParameters[2];
    rootParameters[0].InitAsDescriptorTable(1, &cbvVertexRange, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &cbvPixelRange, D3D12_SHADER_VISIBILITY_PIXEL);
    //rootParameters[2].InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_PIXEL);
    //rootParameters[3].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

    //SamplerDesc samplerDesc;

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ID3DBlob* signature;
    ID3DBlob* error;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), "Failed To Serialize Root Signature");
    
    ThrowIfFailed(GRHI.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Failed To Create Root Signature");
}
