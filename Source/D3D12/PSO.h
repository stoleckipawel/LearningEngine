#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "ShaderCompiler.h"
#include "Geometry.h"


// Describes depth test configuration for the pipeline state
struct D3D12_DEPTH_TEST_DESC
{
	bool DepthEnable;
	D3D12_DEPTH_WRITE_MASK DepthWriteMask;
	D3D12_COMPARISON_FUNC DepthFunc;
};

// Describes stencil test configuration for the pipeline state.
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

// Pipeline State Object (PSO) class for configuring and managing D3D12 pipeline state.
class PSO
{
public:
	//Creates the graphics pipeline state object.
	void Create(Geometry& vertecies, ID3D12RootSignature* rootSignature, ShaderCompiler& vertexShader, ShaderCompiler& pixelShader);

	//Sets the pipeline state object for the current command list.
	void Set();

	//Returns the underlying pipeline state COM pointer.
	ComPointer<ID3D12PipelineState> Get() { return m_pso; }
private:
	void SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
	void SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode);
	void SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc);
	void SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc);
	void SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc);
private:
	ComPointer<ID3D12PipelineState> m_pso = nullptr; // Pipeline state COM pointer
};

