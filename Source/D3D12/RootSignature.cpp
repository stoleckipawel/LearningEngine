#include "RootSignature.h"

void FRootSignature::Create()
{
    /*
    D3D12_DESCRIPTOR_RANGE descriptorTableRanges[1];
    descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    descriptorTableRanges[0].NumDescriptors = 1;
    descriptorTableRanges[0].BaseShaderRegister = 0;
    descriptorTableRanges[0].RegisterSpace = 0;
    descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
    descriptorTable.NumDescriptorRanges = 1;
    descriptorTable.pDescriptorRanges = descriptorTableRanges;

    D3D12_ROOT_PARAMETER rootParameters[1] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].DescriptorTable = descriptorTable;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    */

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.pParameters = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* signature;
    ID3DBlob* error;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), "Failed To Serialize Root Signature");
    
    ThrowIfFailed(GRHI.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Failed To Create Root Signature");
}
