#include "Core/PCH.h"
#include "D3D12/RootSignature.h"

// Constructs and creates the root signature for the graphics pipeline
RootSignature::RootSignature()
{
    Create();
}

// Destructor releases the root signature
RootSignature::~RootSignature()
{
    Release();
}

void RootSignature::Release()
{
    m_rootSignature.Release();
}

// Creates the root signature for the graphics pipeline
void RootSignature::Create()
{
    // Descriptor range for vertex shader constant buffer views (CBV)
    CD3DX12_DESCRIPTOR_RANGE cbvVertexRange;
    cbvVertexRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, NumFramesInFlight, 0); // b0

    // Descriptor range for pixel shader constant buffer views (CBV)
    CD3DX12_DESCRIPTOR_RANGE cbvPixelRange;
    cbvPixelRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, NumFramesInFlight, 0); // b0

    // Descriptor range for samplers
    CD3DX12_DESCRIPTOR_RANGE samplerRange;
    samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0); // s0

    // Descriptor range for shader resource views (SRV) for textures
    CD3DX12_DESCRIPTOR_RANGE textureRange;
    textureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

    // Root parameters: texture SRV, sampler, vertex CBV, pixel CBV
    CD3DX12_ROOT_PARAMETER rootParameters[4];
    rootParameters[0].InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[1].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsDescriptorTable(1, &cbvVertexRange, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[3].InitAsDescriptorTable(1, &cbvPixelRange, D3D12_SHADER_VISIBILITY_PIXEL);

    // Root signature description
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Init(
        _countof(rootParameters),
        rootParameters,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );

    // Serialize and create the root signature
    ID3DBlob* signature = nullptr;
    ID3DBlob* error = nullptr;
    ThrowIfFailed(
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
        "Failed To Serialize Root Signature"
    );

    ThrowIfFailed(
        GRHI.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)),
        "Failed To Create Root Signature"
    );
}
