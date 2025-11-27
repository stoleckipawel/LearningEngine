#pragma once

#include "ShaderCompiler.h"
#include "Texture.h"
#include "Geometry.h"
#include "PSO.h"
#include "RootSignature.h"
#include "ConstantBuffer.h"
#include "Sampler.h"
#include "DepthStencil.h"

// Renderer manages the graphics pipeline, resources, and main render loop
class Renderer
{
public:
	// Loads all resources and initializes the rendering pipeline
	void Load();

	// Releases all resources and subsystems
	void Release();

	// Shuts down the renderer and all subsystems
	void Shutdown();

	// Main render loop for the scene
	void OnRender();

	// Handles window resize events
	void OnResize();

private:
	void LoadGeometry();
	void LoadTextures();
	void LoadSamplers();
	void LoadShaders();
	void PostLoad();

	void CreateRootSignatures();
	void CreatePSOs();
	void CreateFrameBuffers();
	void SetViewport();
	void SetBackBufferRTV();

	void BindDescriptorTables();
	void PopulateCommandList();

	void OnUpdate();

private:
	std::unique_ptr<Texture> m_texture;
	std::unique_ptr<DepthStencil> m_depthStencil;
	std::unique_ptr<Sampler> m_sampler;
	std::unique_ptr<Geometry> m_vertecies; 
	std::unique_ptr<PSO> m_pso;
	std::unique_ptr<RootSignature> m_rootSignature;
	std::unique_ptr<ShaderCompiler> m_vertexShader;
	std::unique_ptr<ShaderCompiler> m_pixelShader;
	UINT m_frameIndex = 0;
};

// Global renderer instance
extern Renderer GRenderer;

