#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12ImageLoader.h"
#include <vector>
#include "../Shader/Shader.h"


struct Vertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
};

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

class D3D12Renderer
{
public:
	bool Initialize();
	void Setup();
	void Shutdown();
	void Update();

private:
	void Draw(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	
	//Vertex
	void UploadVertecies();
	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();

	//Texture
	D3D12_RESOURCE_DESC CreateTextureResourceDesc(D3D12ImageLoader::ImageData& textureData);

	//PSO
	void SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
	void SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode);
	void SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc);
	void SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc);
	void SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc);
	void CreateRootSignature();
	void CreatePSO();
	void SetPSO(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	
	//Misc
	void ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetShaderParams(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	

private:
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPointer<ID3D12Resource2> vertexBuffer = nullptr;
	ComPointer<ID3D12RootSignature> rootSignature = nullptr;
	ComPointer<ID3D12PipelineState> pso = nullptr;
};

