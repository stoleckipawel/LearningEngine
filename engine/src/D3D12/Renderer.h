
#pragma once

// Forward declarations
class ShaderCompiler;
class Texture;
class Geometry;
class PSO;
class RootSignature;
class Sampler;
class DepthStencil;

// -----------------------------------------------------------------------------
// Renderer: Main graphics pipeline manager for the engine
// -----------------------------------------------------------------------------
// - Owns all major graphics resources and pipeline objects
// - Orchestrates initialization, per-frame updates, rendering, and shutdown
// -----------------------------------------------------------------------------
class Renderer
{
public:
	void Initialize();

	// Releases graphics resources 
	void Reset();

	// Shuts down the renderer and all owned subsystems
	void Shutdown();

	// Main render loop: called once per frame
	void OnRender();

	// Handles window resize events (recreates frame buffers)
	void OnResize();

private:
	// Loads graphics resources
	void Load();
	// Loads geometry (vertex/index buffers)
	void LoadGeometry();
	// Loads texture resources
	void LoadTextures();
	// Loads/initializes sampler states
	void LoadSamplers();
	// Compiles and loads shaders
	void LoadShaders();
	// Finalizes resource uploads and flushes command queue
	void PostLoad();

	void CreateRootSignatures();
	void CreatePSOs();
	void CreateFrameBuffers();
	void SetViewport();
	void SetBackBufferRTV();

	// Binds descriptor tables for textures, samplers, and constant buffers
	void BindDescriptorTables();
	// Records all rendering commands for the current frame
	void PopulateCommandList();

	// Updates per-frame data and constant buffers
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

