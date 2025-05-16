#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12ImageLoader.h"
#include <vector>
#include "D3D12ShaderCompiler.h"
#include "D3D12Texture.h"
#include "D3D12Geometry.h"
#include "D3D12PSO.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12RootSignature.h"

class D3D12Renderer
{
public:
	bool Initialize();
	void Load();
	void Shutdown();
	void Render();

private:
	void LoadGeometry();
	void LoadTextures();
	void LoadShaders();
	void LoadRootSignature();
	void CreateDescriptorHeaps();
	void CreatePSO();

	void CreateDepthStencilBuffer();

	void SetDescriptorHeaps(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetShaderParams(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

	void Draw(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void CreateFrameBuffers();

private:
	D3D12Texture texture;

	D3D12Geometry vertecies;

	D3D12PSO pso;

	D3D12RootSignature rootSignature;

	D3D12DescriptorHeap srvHeap = D3D12DescriptorHeap();
	D3D12DescriptorHeap samplerHeap = D3D12DescriptorHeap();
	D3D12DescriptorHeap dsvHeap = D3D12DescriptorHeap();

	D3D12_RESOURCE_DESC depthStencilResourceDesc;
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	ComPointer<ID3D12Resource> depthStencilBuffer;


	D3D12ShaderCompiler vertexShader;
	D3D12ShaderCompiler pixelShader;
};


