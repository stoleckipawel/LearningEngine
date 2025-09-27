#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "ShaderCompiler.h"
#include "Texture.h"
#include "Geometry.h"
#include "PSO.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"

struct FConstantBufferData
{
	XMFLOAT4 color;
};

class FRenderer
{
public:
	bool Initialize();
	void Load();
	void Unload();
	void Release();
	void Shutdown();
	void OnRender();
	void OnResize();
private:
	void LoadGeometry();
	void UnloadGeometry();

	void LoadTextures();
	void UnloadTextures();

	void LoadShaders();
	void LoadAssets();

	void CreateRootSignatures();
	void ReleaseRootSignatures();

	void CreateDescriptorHeaps();
	void ReleaseDescriptorHeaps();

	void CreatePSOs();
	void ReleasePSOs();

	void CreateDepthStencilBuffer();
	void CreateFrameBuffers();
	void ReleaseFrameBuffers();
	
	void CreateConstantBuffers(FDescriptorHeap& descriptorHeap);
	void ReleaseConstantBuffers();

	void SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

	void OnFrameBegin();
	void OnFrameEnd();
	void PopulateCommandList();
	
	void UpdateRainbowColor();
	void OnUpdate();
private:
	FTexture texture;
	FGeometry vertecies;
	FPSO pso;
	FRootSignature rootSignature;
	FDescriptorHeap cbvHeap = FDescriptorHeap();
	FDescriptorHeap samplerHeap = FDescriptorHeap();
	FDescriptorHeap dsvHeap = FDescriptorHeap();
	D3D12_RESOURCE_DESC depthStencilResourceDesc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
	ComPointer<ID3D12Resource> depthStencilBuffer = nullptr;

	FConstantBuffer<FConstantBufferData> ConstantBuffer;
	FShaderCompiler vertexShader;
	FShaderCompiler pixelShader;

	UINT FrameIndex = 0;
public:
	bool bInitialized = false;
};

extern FRenderer GRenderer;

