#include "RootSignature.h"

void FRootSignature::Create()
{
    CD3DX12_DESCRIPTOR_RANGE vertexRange;
    vertexRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, BufferingCount, 0); //b0 -> Vertex Constant Buffer

    CD3DX12_DESCRIPTOR_RANGE pixelRange;
    pixelRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, BufferingCount, 0); //b0 -> Pixel Constant Buffer

    CD3DX12_ROOT_PARAMETER rootParameters[2];
    rootParameters[0].InitAsDescriptorTable(1, &vertexRange, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &pixelRange, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ID3DBlob* signature;
    ID3DBlob* error;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), "Failed To Serialize Root Signature");
    
    ThrowIfFailed(GRHI.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Failed To Create Root Signature");
}
