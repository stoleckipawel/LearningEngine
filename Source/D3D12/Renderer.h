#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "ShaderCompiler.h"
#include "Texture.h"
#include "Geometry.h"
#include "PSO.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "Sampler.h"
#include "DepthStencil.h"

class Renderer
{
public:
	void Load();
	void Release();
	void Shutdown();
	void OnRender();
	void OnResize();
private:
	void LoadGeometry();
	void LoadTextures();
	void LoadSamplers();
	void LoadShaders();
	void PostLoad();

	void CreateRootSignatures();
	void ReleaseRootSignatures();

	void CreatePSOs();
	void ReleasePSOs();

	void CreateFrameBuffers();
	void SetViewport();
	void SetBackBufferRTV();

	void BindDescriptorTables();
	void PopulateCommandList();

	void OnUpdate();
private:
	Texture m_texture;
	DepthStencil m_depthStencil;
	Sampler m_sampler;
	Geometry m_vertecies;
	PSO m_pso;
	RootSignature m_rootSignature;
	ShaderCompiler m_vertexShader;
	ShaderCompiler m_pixelShader;
	UINT m_frameIndex = 0;
};

extern Renderer GRenderer;

