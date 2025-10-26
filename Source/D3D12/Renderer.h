#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "ShaderCompiler.h"
#include "Texture.h"
#include "Geometry.h"
#include "PSO.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "SamplerDesc.h"




class Renderer
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

	void LoadSamplers();

	void LoadShaders();

	void CreateRootSignatures();
	void ReleaseRootSignatures();

	void ReleaseDescriptorHeaps();

	void CreatePSOs();
	void ReleasePSOs();

	void CreateCommandLists();
	void ReleaseCommandLists();

	void CreateDepthStencilBuffer();
	void CreateFrameBuffers();
	void ReleaseFrameBuffers();

	void SetViewport();
	void SetBackBufferRTV();

	void ClearBackBuffer();
	void ClearDepthStencilBuffer();
	
	void BindDescriptorTables();

	void PopulateCommandList();
	
	void UpdateRainbowColor();
	void OnUpdate();
private:
	Texture texture;
	SamplerDesc sampler;
	Geometry vertecies;
	PSO pso;
	RootSignature rootSignature;

	D3D12_RESOURCE_DESC depthStencilResourceDesc = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilHandle = {};
	ComPointer<ID3D12Resource> depthStencilBuffer = nullptr;

	ShaderCompiler vertexShader;
	ShaderCompiler pixelShader;

	UINT BackBufferFrameIndex = 0;
};

extern Renderer GRenderer;

