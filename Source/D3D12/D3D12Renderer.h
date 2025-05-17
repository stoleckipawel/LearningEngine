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
#include "D3D12ConstantBuffer.h"

class D3D12Renderer
{
public:
	bool Initialize();
	void Load();
	void Shutdown();
	void OnRender();

private:
	void LoadGeometry();
	void LoadTextures();
	void LoadShaders();
	void LoadRootSignature();
	void CreateDescriptorHeaps();
	void CreatePSO();
	void CreateDepthStencilBuffer();
	void LoadConstantBuffers(D3D12DescriptorHeap& descriptorHeap);

	void SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetShaderParams(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

	void PopulateCommandList();
	void CreateFrameBuffers();

	void OnUpdate();

private:
	D3D12Texture texture;

	D3D12Geometry vertecies;

	D3D12PSO pso;

	D3D12RootSignature rootSignature;

	D3D12DescriptorHeap cbvHeap = D3D12DescriptorHeap();
	D3D12DescriptorHeap samplerHeap = D3D12DescriptorHeap();
	D3D12DescriptorHeap dsvHeap = D3D12DescriptorHeap();

	D3D12_RESOURCE_DESC depthStencilResourceDesc;
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	ComPointer<ID3D12Resource> depthStencilBuffer;

	D3D12ConstantBuffer constantBuffer;

	D3D12ShaderCompiler vertexShader;
	D3D12ShaderCompiler pixelShader;
};


