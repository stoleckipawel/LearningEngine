
#include "PCH.h"
#include "D3D12/RootSignature.h"


RootSignature::RootSignature()
{
    Create();
}

// Helper: Sets up all root parameters and descriptor ranges
void RootSignature::SetupRootParameters(CD3DX12_ROOT_PARAMETER* rootParameters, CD3DX12_DESCRIPTOR_RANGE* ranges) {
    // Vertex shader CBV (b0)
    // Pixel shader CBV (b0)
    // Sampler (s0)
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, EngineSettings::FramesInFlight, 0);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, EngineSettings::FramesInFlight, 0);
    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
    // Texture SRV (t0)
    ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    // Root parameters: texture SRV, sampler, vertex CBV, pixel CBV
    rootParameters[0].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[1].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
}

void RootSignature::Create()
{
    // Modular: Setup root parameters and descriptor ranges
    CD3DX12_ROOT_PARAMETER rootParameters[4] = {};
    CD3DX12_DESCRIPTOR_RANGE ranges[4] = {};
    SetupRootParameters(rootParameters, ranges);

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

    ThrowIfFailed(GRHI.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf())), "Failed To Create Root Signature");
    DebugUtils::SetDebugName(m_rootSignature, L"RHI_RootSignature");
}

RootSignature::~RootSignature()
{
    m_rootSignature.Reset();
}