#include "PCH.h"
#include "Renderer.h"
#include "DebugLayer.h"
#include "RHI.h"
#include "Window.h"
#include "ShaderCompiler.h"
#include "Texture.h"
#include "PrimitiveFactory.h"
#include "PSO.h"
#include "RootSignature.h"
#include "RootBindings.h"
#include "ConstantBuffer.h"
#include "ConstantBufferManager.h"
#include "Sampler.h"
#include "DepthStencil.h"
#include "UI.h"
#include "Timer.h"
#include "Camera.h"

// Global renderer instance
Renderer GRenderer;

// -----------------------------------------------------------------------------
// Initializes all graphics subsystems and resources
// -----------------------------------------------------------------------------
void Renderer::Initialize() noexcept
{
    // Initialize the rendering hardware interface (RHI)
    GRHI.Initialize();
    
    // Create root signature first - defines shader resource binding layout
    m_rootSignature = std::make_unique<RootSignature>();

    // Compile shaders
    m_vertexShader = std::make_unique<ShaderCompiler>("SimpleVS.hlsl", "vs_6_0", "main");
    m_pixelShader = std::make_unique<ShaderCompiler>("SimplePS.hlsl", "ps_6_0", "main");

    // Initialize descriptor heap manager and swap chain
    GDescriptorHeapManager.Initialize();
    GSwapChain.Initialize();
    
    // Initialize the global constant buffer manager (manages per-frame CB instances)
    GConstantBufferManager.Initialize();
    
    // Load textures and create sampler
    m_texture = std::make_unique<Texture>(std::filesystem::path("Test1.png"));
    m_sampler = std::make_unique<Sampler>();
    
    // Create geometry
    GatherPrimitives();
    
    // Create pipeline state object
    m_pso = std::make_unique<PSO>(m_primitiveFactory->GetFirstPrimitive(), *m_rootSignature, *m_vertexShader, *m_pixelShader);
    
    // Create depth stencil and other frame buffers
    CreateFrameBuffers();
    
#if USE_GUI
    GUI.Initialize();
#endif
    
    PostLoad();
}

void Renderer::GatherPrimitives()
{
    m_primitiveFactory = std::make_unique<PrimitiveFactory>();


    // Hard-coded 20 cubes with varied translation, rotation, and smaller scale or further from camera
    std::vector<std::tuple<XMFLOAT3, XMFLOAT3, XMFLOAT3>> boxParams = {
        // translation                rotation (radians)           scale
        { { -10.0f,  0.0f,  -5.0f }, { 0.0f, 0.0f, 0.0f }, { 1.2f, 1.2f, 1.2f } },
        { { -8.0f,   2.0f,   6.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.5f, 1.0f } },
        { { -6.0f,  -2.0f,  -8.0f }, { 0.5f, 0.5f, 0.0f }, { 1.5f, 1.0f, 1.2f } },
        { { -4.0f,   0.0f,   8.0f }, { 0.0f, 0.7f, 0.0f }, { 1.3f, 1.3f, 1.3f } },
        { { -2.0f,   2.0f,  -6.0f }, { 1.0f, 0.0f, 0.5f }, { 1.0f, 0.8f, 1.2f } },
        { {  0.0f,  -2.0f,   6.0f }, { 0.0f, 0.0f, 1.0f }, { 0.9f, 1.1f, 1.3f } },
        { {  2.0f,   0.0f,  -8.0f }, { 0.3f, 0.8f, 0.2f }, { 1.2f, 1.2f, 0.8f } },
        { {  4.0f,   4.0f,   8.0f }, { 0.0f, 1.2f, 0.0f }, { 1.0f, 0.7f, 1.5f } },
        { {  6.0f,  -4.0f,  -6.0f }, { 1.0f, 0.5f, 0.0f }, { 0.7f, 1.5f, 1.0f } },
        { {  8.0f,   0.0f,   6.0f }, { 0.7f, 0.0f, 1.0f }, { 1.3f, 1.0f, 1.0f } },
        { { -9.0f,  -3.0f,  10.0f }, { 0.2f, 0.3f, 0.4f }, { 0.8f, 1.0f, 1.2f } },
        { { -7.0f,   3.0f, -10.0f }, { 0.6f, 0.1f, 0.2f }, { 1.1f, 0.9f, 1.0f } },
        { { -5.0f,  -1.0f,   9.0f }, { 0.4f, 0.6f, 0.8f }, { 1.0f, 1.0f, 0.7f } },
        { { -3.0f,   1.0f,  -9.0f }, { 0.9f, 0.2f, 0.3f }, { 0.9f, 1.2f, 1.1f } },
        { { -1.0f,  -3.0f,   8.0f }, { 0.1f, 0.4f, 0.7f }, { 1.2f, 0.8f, 1.0f } },
        { {  1.0f,   3.0f,  -8.0f }, { 0.5f, 0.9f, 0.1f }, { 1.0f, 1.0f, 1.0f } },
        { {  3.0f,  -1.0f,   7.0f }, { 0.8f, 0.3f, 0.6f }, { 0.7f, 1.1f, 1.2f } },
        { {  5.0f,   1.0f,  -7.0f }, { 0.2f, 0.7f, 0.5f }, { 1.1f, 0.9f, 0.8f } },
        { {  7.0f,  -3.0f,   6.0f }, { 0.3f, 0.6f, 0.9f }, { 0.8f, 1.0f, 1.0f } },
        { {  9.0f,   3.0f,  -5.0f }, { 0.7f, 0.2f, 0.4f }, { 1.0f, 0.8f, 1.2f } }
    };

    for (const auto& [translation, rotation, scale] : boxParams)
    {
        m_primitiveFactory->AppendBox(translation, rotation, scale);
    }

    m_primitiveFactory->Upload();
}

// -----------------------------------------------------------------------------
// Finalizes resource uploads and flushes the command queue
// -----------------------------------------------------------------------------
void Renderer::PostLoad() noexcept
{
    GRHI.CloseCommandListScene();
    GRHI.ExecuteCommandList();
    GRHI.Flush();
}

// -----------------------------------------------------------------------------
// Sets viewport and scissor rectangle for rasterization
// -----------------------------------------------------------------------------
void Renderer::SetViewport() noexcept
{
    D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
    GRHI.GetCommandList()->RSSetViewports(1, &viewport);

    D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
    GRHI.GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

// -----------------------------------------------------------------------------
// Sets render target and depth stencil views for output merger
// -----------------------------------------------------------------------------
void Renderer::SetBackBufferRTV() noexcept
{
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetCPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil->GetCPUHandle();
    GRHI.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
}

// -----------------------------------------------------------------------------
// Binds per-frame resources: textures, samplers, and frame/view constant buffers
// These bindings remain constant for all draw calls within a frame.
// -----------------------------------------------------------------------------
void Renderer::BindPerFrameResources() noexcept
{
    // -------------------------------------------------------------------------
    // Bind Per-Frame Constant Buffer - updated once per CPU frame
    // Contains: FrameIndex, TotalTime, DeltaTime, ViewportSize..........
    // -------------------------------------------------------------------------
    if (auto* perFrameCB = GConstantBufferManager.GetPerFrameConstantBuffer())
    {
        GRHI.GetCommandList()->SetGraphicsRootConstantBufferView(
            RootBindings::RootParam::PerFrame,
            perFrameCB->GetGPUVirtualAddress());
    }

    // -------------------------------------------------------------------------
    // Bind Per-View Constant Buffer - updated once per view/camera
    // Contains: View/Proj matrices, CameraPosition, Near/Far planes.............
    // -------------------------------------------------------------------------
    if (auto* perViewCB = GConstantBufferManager.GetPerViewConstantBuffer())
    {
        GRHI.GetCommandList()->SetGraphicsRootConstantBufferView(
            RootBindings::RootParam::PerView,
            perViewCB->GetGPUVirtualAddress());
    }

    // -------------------------------------------------------------------------
    // Bind Textures SRV - descriptor table
    // -------------------------------------------------------------------------
    if (m_texture)
    {
        GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
            RootBindings::RootParam::TextureSRV,
            m_texture->GetGPUHandle());
    }

    // -------------------------------------------------------------------------
    // Bind Samplers - descriptor table
    // -------------------------------------------------------------------------
    if (m_sampler)
    {
        GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
            RootBindings::RootParam::Sampler,
            m_sampler->GetGPUHandle());
    }
}

// -----------------------------------------------------------------------------
// Binds per-object resources before each draw call
// These bindings change for every primitive/mesh being rendered.
// -----------------------------------------------------------------------------
void Renderer::BindPerObjectResources(const Primitive& primitive) noexcept
{
    // -------------------------------------------------------------------------
    // Update and Bind Per-Object VS Constant Buffer (b2)
    // Contains: WorldMatrix for this primitive
    // -------------------------------------------------------------------------
    GConstantBufferManager.UpdatePerObjectVS(primitive);
    if (auto* perObjectVSCB = GConstantBufferManager.GetPerObjectVSConstantBuffer())
    {
        GRHI.GetCommandList()->SetGraphicsRootConstantBufferView(
            RootBindings::RootParam::PerObjectVS,
            perObjectVSCB->GetGPUVirtualAddress());
    }

    // -------------------------------------------------------------------------
    // Update and Bind Per-Object PS Constant Buffer (b3)
    // Contains: Material properties (BaseColor, Metallic, Roughness, F0)
    // TODO: Pass material data from primitive when material system is implemented
    // -------------------------------------------------------------------------
    GConstantBufferManager.UpdatePerObjectPS();
    if (auto* perObjectPSCB = GConstantBufferManager.GetPerObjectPSConstantBuffer())
    {
        GRHI.GetCommandList()->SetGraphicsRootConstantBufferView(
            RootBindings::RootParam::PerObjectPS,
            perObjectPSCB->GetGPUVirtualAddress());
    }
}

// -----------------------------------------------------------------------------
// Records all rendering commands for the current frame
// -----------------------------------------------------------------------------
void Renderer::PopulateCommandList()
{
    // =========================================================================
    // FRAME SETUP - Execute once per frame
    // =========================================================================
    
    // Prepare render target - transition to render target state
    GSwapChain.SetRenderTargetState();

    // Transition depth buffer to write state before rendering
    m_depthStencil->SetWriteState();

    // Bind root signature - defines the shader resource layout
    GRHI.GetCommandList()->SetGraphicsRootSignature(m_rootSignature->GetRaw());

    // Set viewport and scissor rect
    SetViewport();
    
    // Set render targets (back buffer + depth stencil)
    SetBackBufferRTV();
    
    // Clear render targets
    GSwapChain.Clear();
    m_depthStencil->Clear();

    // Set shader-visible descriptor heaps
    GDescriptorHeapManager.SetShaderVisibleHeaps();

    // =========================================================================
    // PER-FRAME BINDINGS - Resources that don't change during the frame
    // =========================================================================
    BindPerFrameResources();

    // =========================================================================
    // DRAW LOOP - Per-object bindings and draw calls
    // =========================================================================
    const auto& primitives = m_primitiveFactory->GetPrimitives();
    
    for (const auto& primitive : primitives)
    {
        // Set geometry buffers (VB, IB) and topology for this primitive
        primitive->Set();
        
        // Set the pipeline state object (PSO)
        // ToDo: sort by PSO to minimize state changes
        m_pso->Set();

        // Bind per-object constant buffers (world matrix, material)
        BindPerObjectResources(*primitive);

        // Issue the draw call for this primitive
        GRHI.GetCommandList()->DrawIndexedInstanced(primitive->GetIndexCount(), 1, 0, 0, 0);
    }

    // =========================================================================
    // POST-RENDER
    // =========================================================================
    
#if USE_GUI
    GUI.Render();
#endif

    // Transition depth buffer to read state before presenting
    m_depthStencil->SetReadState();
    
    // Transition back buffer to present state
    GSwapChain.SetPresentState();
}

// -----------------------------------------------------------------------------
// Creates frame buffers and depth stencil resources
// -----------------------------------------------------------------------------
void Renderer::CreateFrameBuffers()
{
    m_depthStencil = std::make_unique<DepthStencil>();
}

// -----------------------------------------------------------------------------
// Updates per-frame data and constant buffers
// Called once per frame before PopulateCommandList
// -----------------------------------------------------------------------------
void Renderer::OnUpdate()
{
    // =========================================================================
    // TIMING - Update global time source first (other systems depend on it)
    // =========================================================================
    GTimer.Tick();

    // =========================================================================
    // CONSTANT BUFFER UPDATES - Update CB data at appropriate frequencies
    // =========================================================================
    
    // Per-Frame CB (b0): Updated once per CPU frame
    // Contains: FrameIndex, TotalTime, DeltaTime, ViewportSize
    GConstantBufferManager.UpdatePerFrame();
    
    // Per-View CB (b1): Updated once per camera/view
    // Contains: View/Proj matrices, CameraPosition, Near/Far
    // ToDo: Camera matrices are lazily rebuilt when accessed via GCamera.
    //       In a multi-view scenario (shadows, reflections), UpdatePerView()
    //       would be called once per view, not once per frame.
    GConstantBufferManager.UpdatePerView();

#if USE_GUI
    // Pass seconds to UI which expects seconds-precision delta
    GUI.Update();
#endif
}

// -----------------------------------------------------------------------------
// Handles window resize events and recreates frame buffers
// -----------------------------------------------------------------------------
void Renderer::OnResize() noexcept
{
    GRHI.Flush();
    // Resize swap chain to match new window dimensions
    GSwapChain.Resize();
    // Recreate frame buffers (depth stencil) for new size
    CreateFrameBuffers();
}

// -----------------------------------------------------------------------------
// Main render loop: called once per frame
// -----------------------------------------------------------------------------
void Renderer::OnRender() noexcept
{
    // Modular frame sequence
    // 1) Wait / reset / prepare GPU and command allocators
    BeginFrame();

    // 2) Update CPU-side state, timers, and constant buffers
    SetupFrame();

    // 3) Build command list entries (draw calls, dispatches)
    RecordFrame();

    // 4) Submit to GPU and present
    SubmitFrame();

    // 5) Finalize frame bookkeeping (advance indices, cleanup)
    EndFrame();
}

// -----------------------------------------------------------------------------
// BeginFrame: prepare GPU and command allocators for a new frame
// - WaitForGPU may be necessary depending on sync design
// - Reset command allocator / list
// -----------------------------------------------------------------------------
void Renderer::BeginFrame() noexcept
{
    GRHI.WaitForGPU();
    GRHI.ResetCommandAllocator();
    GRHI.ResetCommandList();
}

// -----------------------------------------------------------------------------
// SetupFrame: perform per-frame CPU updates (timing, camera, per-frame CBs)
// This is where heavy CPU work and culling would occur in a full engine.
// -----------------------------------------------------------------------------
void Renderer::SetupFrame() noexcept
{
    OnUpdate(); // existing per-frame update entry
}

// -----------------------------------------------------------------------------
// RecordFrame: record rendering commands into the command list
// This intentionally delegates to PopulateCommandList which contains
// the detailed recording logic. In the future this function will
// orchestrate culling, batching, and multi-pass rendering.
// -----------------------------------------------------------------------------
void Renderer::RecordFrame() noexcept
{
    PopulateCommandList();
}

// -----------------------------------------------------------------------------
// SubmitFrame: close, execute, and present recorded command lists
// -----------------------------------------------------------------------------
void Renderer::SubmitFrame() noexcept
{
    GRHI.CloseCommandListScene();
    GRHI.ExecuteCommandList();
    GRHI.Signal();
    GSwapChain.Present();
}

// -----------------------------------------------------------------------------
// EndFrame: advance frame indices and perform end-of-frame housekeeping
// -----------------------------------------------------------------------------
void Renderer::EndFrame() noexcept
{
    GSwapChain.UpdateFrameInFlightIndex();
}

// -----------------------------------------------------------------------------
// Shuts down the renderer and all owned subsystems
// -----------------------------------------------------------------------------
void Renderer::Shutdown() noexcept
{
    GRHI.Flush();       
    GUI.Shutdown();    
    GSwapChain.Shutdown();
    GWindow.Shutdown();
    GDescriptorHeapManager.Shutdown();
    GRHI.Shutdown();
}