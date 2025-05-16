#include "D3D12RootSignature.h"

bool D3D12RootSignature::Create()
{
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pParameters = nullptr;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* signature;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);

    if (FAILED(hr))
    {
        std::string message = "Root Signature Create: Failed To Serialize Root Signature";
        LogError(message);
        return false;
    }

    hr = D3D12Context::Get().GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    if (FAILED(hr))
    {
        std::string message = "Root Signature Create: Failed To Create Root Signature";    
        LogError(message);
        return false;
    }

    return true;
}
