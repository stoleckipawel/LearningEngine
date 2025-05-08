#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12ImageLoader.h"
#include <vector>

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

	void Shutdown();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();

	D3D12_RESOURCE_DESC CreateVertexBufferDesc(uint32_t VertexCount);
	D3D12_RESOURCE_DESC CreateTextureResourceDesc(D3D12ImageLoader::ImageData& textureData);
	void SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc);
	void SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode);
	void SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc);
	void SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc);
	void SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc);

	void LoadContent();

	//Singleton pattern to ensure only one instance of the debug layer exists
public:
	D3D12Renderer(const D3D12Renderer&) = delete;
	D3D12Renderer& operator=(const D3D12Renderer&) = delete;

	inline static D3D12Renderer& Get()
	{
		static D3D12Renderer instance;
		return instance;
	}
private:
	D3D12Renderer() = default;
};

