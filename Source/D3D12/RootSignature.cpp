#include "RootSignature.h"

bool FRootSignature::Create()
{
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

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 1;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    ID3DBlob* signature;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);

    if (FAILED(hr))
    {
        std::string message = "Root Signature Create: Failed To Serialize Root Signature";
        LogError(message, ELogType::Fatal);
        return false;
    }

    hr = GRHI.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    if (FAILED(hr))
    {
        std::string message = "Root Signature Create: Failed To Create Root Signature";    
        LogError(message, ELogType::Fatal);
        return false;
    }

    return true;
}
