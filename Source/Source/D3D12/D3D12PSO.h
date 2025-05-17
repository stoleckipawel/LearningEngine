#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12ShaderCompiler.h"
#include "D3D12Geometry.h"

struct D3D12_DEPTH_TEST_DESC
{
	bool DepthEnable;
	D3D12_DEPTH_WRITE_MASK DepthWriteMask;
	D3D12_COMPARISON_FUNC DepthFunc;
};

struct D3D12_STENCIL_TEST_DESC
{
	bool StencilEnable;
	uint8_t StencilReadMask;
	uint8_t StencilWriteMask;
	D3D12_COMPARISON_FUNC FrontFaceStencilFunc;
	D3D12_STENCIL_OP FrontFaceStencilFailOp;
	D3D12_STENCIL_OP FrontFaceStencilDepthFailOp;
	D3D12_STENCIL_OP FrontFaceStencilPassOp;
	D3D12_COMPARISON_FUNC BackFaceStencilFunc;
	D3D12_STENCIL_OP BackFaceStencilFailOp;
	D3D12_STENCIL_OP BackFaceStencilDepthFailOp;
	D3D12_STENCIL_OP BackFaceStencilPassOp;
};

class D3D12PSO
{
public:
	void Create(D3D12Geometry& vertecies, ID3D12RootSignature* rootSignature, D3D12ShaderCompiler& vertexShader, D3D12ShaderCompiler& pixelShader);
	void Set(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

	ComPointer<ID3D12PipelineState> pso = nullptr;
	void SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
	void SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode);
	void SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc);
	void SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc);
	void SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc);
	
};

