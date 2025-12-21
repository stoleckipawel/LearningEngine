
#pragma once

// Forward declarations
class ShaderCompiler;
class Texture;
class PrimitiveFactory;
class PSO;
class RootSignature;
class Sampler;
class DepthStencil;
class Primitive;

// -----------------------------------------------------------------------------
// Renderer: Main graphics pipeline manager for the engine
// -----------------------------------------------------------------------------
// Responsibilities:
//   - Owns all major graphics resources and pipeline objects
//   - Orchestrates initialization, per-frame updates, rendering, and shutdown
//   - Manages constant buffer update frequencies (per-frame, per-view, per-object)
//   - Records and submits command lists to the GPU
//
// Binding Frequency Optimization (following Frostbite/Unreal patterns):
//   - Per-Frame resources (b0, textures, samplers): bound once at frame start
//   - Per-View resources (b1): bound once per camera/view
//   - Per-Object resources (b2, b3): bound before each draw call
// -----------------------------------------------------------------------------
class Renderer
{
public:
    // Initialize all graphics subsystems and resources
    void Initialize() noexcept;

    // Shut down the renderer and release all owned resources
    void Shutdown() noexcept;

    // Main render loop entry point: called once per frame
    void OnRender() noexcept;

    // Handle window resize events (recreates swap chain and frame buffers)
    void OnResize() noexcept;

private:
    // -------------------------------------------------------------------------
    // Initialization helpers
    // -------------------------------------------------------------------------
    
    // Finalize resource uploads and flush command queue after init
    void PostLoad() noexcept;
    
    // Create depth stencil and other frame buffer resources
    void CreateFrameBuffers();
    
    // Populate primitive factory with scene geometry
    void GatherPrimitives();

    // -------------------------------------------------------------------------
    // Per-frame update
    // -------------------------------------------------------------------------
    
    // Update per-frame data: timing, camera, constant buffers
    void OnUpdate();

    // -------------------------------------------------------------------------
    // Command list recording
    // -------------------------------------------------------------------------
    
    // Record all rendering commands for the current frame
    void PopulateCommandList();

    // Called at start of frame to wait, reset allocators, and prepare GPU
    void BeginFrame() noexcept;

    // Prepare per-frame CPU-side state (timing, camera, per-frame CB updates)
    void SetupFrame() noexcept;

    // Record draw/dispatch work into the command list
    void RecordFrame() noexcept;

    // Submit recorded command lists and present
    void SubmitFrame() noexcept;

    // Finalize frame: advance frame index, perform end-of-frame housekeeping
    void EndFrame() noexcept;
    
    // Set viewport and scissor rectangle for rasterization
    void SetViewport() noexcept;
    
    // Set render target (back buffer) and depth stencil for output merger
    void SetBackBufferRTV() noexcept;
    
    // Bind resources that remain constant for all draws in a frame
    // (per-frame CB, per-view CB, textures, samplers)
    void BindPerFrameResources() noexcept;
    
    // Bind per-object resources before each draw call
    // (world matrix CB, material CB)
    void BindPerObjectResources(const Primitive& primitive) noexcept;

private:
    // -------------------------------------------------------------------------
    // Owned resources (unique_ptr for RAII and clear ownership)
    // -------------------------------------------------------------------------
    std::unique_ptr<Texture> m_texture;
    std::unique_ptr<DepthStencil> m_depthStencil;
    std::unique_ptr<Sampler> m_sampler;
    std::unique_ptr<PrimitiveFactory> m_primitiveFactory;
    std::unique_ptr<PSO> m_pso;
    std::unique_ptr<RootSignature> m_rootSignature;
    std::unique_ptr<ShaderCompiler> m_vertexShader;
    std::unique_ptr<ShaderCompiler> m_pixelShader;
};

// Global renderer instance
extern Renderer GRenderer;

