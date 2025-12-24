#pragma once

#include "DxcShaderCompiler.h"
#include "Primitive.h"
#include "D3D12RootSignature.h"

using Microsoft::WRL::ComPtr;

// Describes depth test configuration for the pipeline state.
// Use short, unambiguous names (avoid colliding with D3D12_* typedefs).
struct DepthTestDesc
{
	bool DepthEnable = true;
	D3D12_DEPTH_WRITE_MASK DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	D3D12_COMPARISON_FUNC DepthFunc = D3D12_COMPARISON_FUNC_LESS;
};

// Describes stencil test configuration for the pipeline state.
struct StencilTestDesc
{
	bool StencilEnable = false;
	uint8_t StencilReadMask = 0xFF;
	uint8_t StencilWriteMask = 0xFF;
	D3D12_COMPARISON_FUNC FrontFaceStencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	D3D12_STENCIL_OP FrontFaceStencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12_STENCIL_OP FrontFaceStencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12_STENCIL_OP FrontFaceStencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12_COMPARISON_FUNC BackFaceStencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	D3D12_STENCIL_OP BackFaceStencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12_STENCIL_OP BackFaceStencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12_STENCIL_OP BackFaceStencilPassOp = D3D12_STENCIL_OP_KEEP;
};

// D3D12PipelineState owns a graphics pipeline state object and the configuration needed to build it.
class D3D12PipelineState
{
public:
	// Constructs and creates the graphics pipeline state object.
	D3D12PipelineState(Primitive& vertecies, D3D12RootSignature& rootSignature, DxcShaderCompiler& vertexShader, DxcShaderCompiler& pixelShader);

	// Destructor releases pipeline state object
	~D3D12PipelineState() noexcept;

	// Deleted copy constructor and assignment operator to enforce unique ownership
	D3D12PipelineState(const D3D12PipelineState&) = delete;
	D3D12PipelineState& operator=(const D3D12PipelineState&) = delete;

	// Sets the pipeline state object for the current command list.
	void Set() const noexcept;

	// Returns a const reference to the underlying pipeline state COM pointer.
	const ComPtr<ID3D12PipelineState>& Get() const noexcept { return m_pso; }
private:
	void SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) noexcept;
	void SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode) noexcept;
	void SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc) noexcept;
	void SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, DepthTestDesc depthDesc) noexcept;
	void SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, StencilTestDesc stencilDesc) noexcept;
private:
	ComPtr<ID3D12PipelineState> m_pso = nullptr; // Pipeline state COM pointer
};

