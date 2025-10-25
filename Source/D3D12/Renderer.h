#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "ShaderCompiler.h"
#include "Texture.h"
#include "Geometry.h"
#include "PSO.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"

struct alignas(256) FVertexConstantBuffer
{
	XMFLOAT4X4 WorldMTX;
	XMFLOAT4X4 ViewMTX;
	XMFLOAT4X4 ProjectionMTX;
};

struct alignas(256) FPixelConstantBuffer
{
	XMFLOAT4 Color;
};

class FRenderer
{
public:
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

	void CreateCommandLists();
	void ReleaseCommandLists();

	void CreateDepthStencilBuffer();
	void CreateFrameBuffers();
	void ReleaseFrameBuffers();
	
	void CreateConstantBuffers();
	void ReleaseConstantBuffers();

	void SetViewport();

	void SetBackBufferRTV();

	void ClearBackBuffer();
	void ClearDepthStencilBuffer();

	void SetDescriptorHeaps();
	void BindDescriptorTables();

	void PopulateCommandList();
	
	void UpdateRainbowColor();
	void OnUpdate();
private:
	FTexture texture;
	FGeometry vertecies;
	FPSO pso;
	FRootSignature rootSignature;
	FDescriptorHeap ConstantBufferHeap = FDescriptorHeap();
	FDescriptorHeap ShaderResourceViewHeap = FDescriptorHeap();
	FDescriptorHeap SamplerHeap = FDescriptorHeap();
	FDescriptorHeap DepthStencilViewHeap = FDescriptorHeap();

	D3D12_RESOURCE_DESC depthStencilResourceDesc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilHandle = {};
	ComPointer<ID3D12Resource> depthStencilBuffer = nullptr;

	FConstantBuffer<FVertexConstantBuffer> VertexConstantBuffers[BufferingCount];
	FConstantBuffer<FPixelConstantBuffer> PixelConstantBuffers[BufferingCount];

	FShaderCompiler vertexShader;
	FShaderCompiler pixelShader;
	UINT NumConstantBuffers = 2;

	UINT FrameIndex = 0;
};

extern FRenderer GRenderer;

