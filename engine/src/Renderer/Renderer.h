
#pragma once

#include "ShaderCompileResult.h"
#include <memory>

// Forward declarations
class Texture;
class D3D12PipelineState;
class D3D12RootSignature;
class D3D12SamplerLibrary;
class D3D12DepthStencil;
class Mesh;

// Renderer orchestrates the graphics pipeline.
class Renderer
{
  public:
	void Initialize() noexcept;
	void Shutdown() noexcept;
	void OnRender() noexcept;
	void OnResize() noexcept;

  private:
	void PostLoad() noexcept;
	void CreateFrameBuffers();

	void PopulateCommandList();
	void BeginFrame() noexcept;
	void SetupFrame() noexcept;
	void RecordFrame() noexcept;
	void SubmitFrame() noexcept;
	void EndFrame() noexcept;

	void SetViewport() noexcept;
	void SetBackBufferRTV() noexcept;
	void BindPerFrameResources() noexcept;
	void BindPerObjectResources(const Mesh& mesh) noexcept;

  private:
	// Textures
	std::unique_ptr<Texture> m_checkerTexture;
	std::unique_ptr<Texture> m_skyCubemapTexture;

	// Frame buffers
	std::unique_ptr<D3D12DepthStencil> m_depthStencil;

	// Sampler library
	std::unique_ptr<D3D12SamplerLibrary> m_samplerLibrary;

	// Pipeline
	std::unique_ptr<D3D12PipelineState> m_pso;
	std::unique_ptr<D3D12RootSignature> m_rootSignature;

	// Compiled shaders (owned bytecode)
	ShaderCompileResult m_vertexShader;
	ShaderCompileResult m_pixelShader;
};

extern Renderer GRenderer;
