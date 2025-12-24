
#include "PCH.h"
#include "D3D12RootSignature.h"
#include "D3D12RootBindings.h"
#include "DebugUtils.h"

D3D12RootSignature::D3D12RootSignature()
{
	Create();
}

void D3D12RootSignature::Create()
{
	// -------------------------------------------------------------------------
	// Root Signature Layout - Defines shader resource binding points
	// -------------------------------------------------------------------------
	// Uses root CBVs for constant buffers (low overhead, direct GPU VA binding)
	// Uses descriptor tables for SRVs/Samplers (flexible, heap-based)
	//
	// See RootBindings.h for the canonical binding slot definitions.
	// -------------------------------------------------------------------------

	CD3DX12_ROOT_PARAMETER rootParameters[RootBindings::RootParam::Count] = {};

	// Descriptor ranges for tables
	CD3DX12_DESCRIPTOR_RANGE ranges[2] = {};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, RootBindings::SRVRegister::BaseTexture);        // t0: texture
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, RootBindings::SamplerRegister::LinearWrap); // s0: sampler

	// -------------------------------------------------------------------------
	// Root CBVs: Direct GPU virtual address binding for constant buffers
	// Low overhead: no descriptor heap indirection, just a 64-bit GPU VA
	// -------------------------------------------------------------------------

	// b0 - PerFrame
	rootParameters[RootBindings::RootParam::PerFrame].InitAsConstantBufferView(
	    RootBindings::CBRegister::PerFrame, 0, RootBindings::Visibility::PerFrame);

	// b1 - PerView
	rootParameters[RootBindings::RootParam::PerView].InitAsConstantBufferView(
	    RootBindings::CBRegister::PerView, 0, RootBindings::Visibility::PerView);

	// b2 - PerObjectVS
	rootParameters[RootBindings::RootParam::PerObjectVS].InitAsConstantBufferView(
	    RootBindings::CBRegister::PerObjectVS, 0, RootBindings::Visibility::PerObjectVS);

	// b3 - PerObjectPS
	rootParameters[RootBindings::RootParam::PerObjectPS].InitAsConstantBufferView(
	    RootBindings::CBRegister::PerObjectPS, 0, RootBindings::Visibility::PerObjectPS);

	// -------------------------------------------------------------------------
	// Descriptor Tables: Heap-based binding for textures and samplers
	// More flexible but requires descriptor heap management
	// -------------------------------------------------------------------------

	rootParameters[RootBindings::RootParam::TextureSRV].InitAsDescriptorTable(
	    1, &ranges[0], RootBindings::Visibility::TextureSRV);

	rootParameters[RootBindings::RootParam::Sampler].InitAsDescriptorTable(
	    1, &ranges[1], RootBindings::Visibility::Sampler);

	// -------------------------------------------------------------------------
	// Create the root signature
	// -------------------------------------------------------------------------

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(RootBindings::RootParam::Count,
	                       rootParameters,
	                       0,
	                       nullptr,
	                       D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Serialize and create the root signature
	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	CHECK(GD3D12Rhi.GetDevice()->CreateRootSignature(0,
	                                                 signature->GetBufferPointer(),
	                                                 signature->GetBufferSize(),
	                                                 IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf())));
	DebugUtils::SetDebugName(m_rootSignature, L"RHI_RootSignature");
}

D3D12RootSignature::~D3D12RootSignature()
{
	m_rootSignature.Reset();
}