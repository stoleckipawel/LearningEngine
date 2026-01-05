// =============================================================================
// Renderer.h — High-Level Graphics Pipeline Orchestration
// =============================================================================
//
// Central rendering subsystem that manages the graphics pipeline, resource
// binding, and frame submission. Acts as the primary interface between game
// logic and the D3D12 RHI layer.
//
// USAGE:
//   GRenderer.Initialize();    // Call once at startup after Window/RHI init
//   GRenderer.OnRender();      // Call each frame from the main loop
//   GRenderer.OnResize();      // Call when window dimensions change
//   GRenderer.Shutdown();      // Call at application exit
//
// RESPONSIBILITIES:
//   - Pipeline state object (PSO) creation and management
//   - Per-frame and per-object constant buffer binding
//   - Depth buffer management and depth mode switching
//   - Scene traversal and mesh rendering
//   - Integration with UI overlay rendering
//
// NOTES:
//   - Singleton accessed via GRenderer global
//   - Initialize() must be called after Window and D3D12Rhi initialization
//   - Owns shader bytecode, textures, and pipeline objects
//
// =============================================================================

#pragma once

#include "Event.h"
#include "ShaderCompileResult.h"
#include <cstdint>
#include <memory>

enum class DepthMode : std::uint8_t;

// Forward declarations
class Texture;
class D3D12PipelineState;
class D3D12RootSignature;
class D3D12SamplerLibrary;
class D3D12DepthStencil;
class Mesh;

// =============================================================================
// Renderer
// =============================================================================

class Renderer final
{
  public:
	[[nodiscard]] static Renderer& Get() noexcept;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	// =========================================================================
	// Lifecycle
	// =========================================================================

	// Initializes all rendering resources: shaders, textures, PSO, depth buffer.
	// Must be called after Window and D3D12Rhi are initialized.
	void Initialize() noexcept;

	// Releases all rendering resources. Call during application shutdown.
	void Shutdown() noexcept;

	// =========================================================================
	// Frame Operations
	// =========================================================================

	// Executes a complete render frame: setup, scene traversal, UI, submission.
	void OnRender() noexcept;

	// Handles window resize: recreates depth buffer and updates viewport.
	void OnResize() noexcept;

  private:
	Renderer() = default;
	~Renderer() = default;

	// -------------------------------------------------------------------------
	// Initialization Helpers
	// -------------------------------------------------------------------------

	void PostLoad() noexcept;
	void CreateDepthStencilBuffer();
	void CreatePSO();
	void OnDepthModeChanged(DepthMode mode) noexcept;

	// -------------------------------------------------------------------------
	// Frame Pipeline Stages
	// -------------------------------------------------------------------------

	void PopulateCommandList();
	void BeginFrame() noexcept;
	void SetupFrame() noexcept;
	void RecordFrame() noexcept;
	void SubmitFrame() noexcept;
	void EndFrame() noexcept;

	// -------------------------------------------------------------------------
	// Resource Binding
	// -------------------------------------------------------------------------

	void SetViewport() noexcept;
	void SetBackBufferRTV() noexcept;
	void BindPerFrameResources() noexcept;
	void BindPerObjectResources(const Mesh& mesh) noexcept;

	// -------------------------------------------------------------------------
	// Owned Resources
	// -------------------------------------------------------------------------

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

	// Event subscriptions
	EventHandle m_depthModeChangedHandle;
};

inline Renderer& GRenderer = Renderer::Get();
