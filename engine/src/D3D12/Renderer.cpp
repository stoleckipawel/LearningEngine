
#include "PCH.h"
#include "Renderer.h"
#include "D3D12/DebugLayer.h"
#include "D3D12/RHI.h"
#include "Window.h"
#include "D3D12/ShaderCompiler.h"
#include "D3D12/Texture.h"
#include "D3D12/PrimitiveFactory.h"
#include "D3D12/PSO.h"
#include "D3D12/RootSignature.h"
#include "D3D12/ConstantBuffer.h"
#include "D3D12/Sampler.h"
#include "D3D12/DepthStencil.h"
#include "D3D12/UI.h"

// Global renderer instance
Renderer GRenderer;

// -----------------------------------------------------------------------------
// Initializes all graphics subsystems and resources
// -----------------------------------------------------------------------------
void Renderer::Initialize()
{
    // Initialize the rendering hardware interface (RHI)
    GRHI.Initialize();
    m_rootSignature = std::make_unique<RootSignature>();
    m_vertexShader = std::make_unique<ShaderCompiler>("SimpleVS.hlsl", "vs_6_0", "main");
    m_pixelShader = std::make_unique<ShaderCompiler>("SimplePS.hlsl", "ps_6_0", "main");
    GDescriptorHeapManager.Initialize();
    GSwapChain.Initialize();
    m_texture = std::make_unique<Texture>(std::filesystem::path("Test1.png"));
    m_sampler = std::make_unique<Sampler>();
    GatherPrimitives();
    m_pso = std::make_unique<PSO>(m_primitiveFactory->GetFirstPrimitive(), *m_rootSignature, *m_vertexShader, *m_pixelShader);
    CreateFrameBuffers();
    GUI.Initialize();
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
void Renderer::PostLoad()
{
    GRHI.CloseCommandListScene();
    GRHI.ExecuteCommandList();
    GRHI.Flush();
}

// -----------------------------------------------------------------------------
// Sets viewport and scissor rectangle for rasterization
// -----------------------------------------------------------------------------
void Renderer::SetViewport()
{
    D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
    GRHI.GetCommandList()->RSSetViewports(1, &viewport);

    D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
    GRHI.GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

// -----------------------------------------------------------------------------
// Sets render target and depth stencil views for output merger
// -----------------------------------------------------------------------------
void Renderer::SetBackBufferRTV()
{
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetCPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil->GetCPUHandle();
    GRHI.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
}

// -----------------------------------------------------------------------------
// Binds descriptor tables for textures, samplers, and constant buffers
// -----------------------------------------------------------------------------
void Renderer::BindDescriptorTables()
{
    // Texture
    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        0,
        m_texture->GetGPUHandle());

    // Sampler
    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        1,
        m_sampler->GetGPUHandle());
}

// -----------------------------------------------------------------------------
// Records all rendering commands for the current frame
// -----------------------------------------------------------------------------
void Renderer::PopulateCommandList()
{
    // Prepare render target
    GSwapChain.SetRenderTargetState();

    // Transition depth buffer to write state before rendering
    m_depthStencil->SetWriteState();

    // Bind root signature
    GRHI.GetCommandList()->SetGraphicsRootSignature(m_rootSignature->GetRaw());

    // Set viewport and render targets
    SetViewport();
    SetBackBufferRTV();
    GSwapChain.Clear();

    // Clear depth stencil and set geometry
    m_depthStencil->Clear();

    GDescriptorHeapManager.SetShaderVisibleHeaps();

    // Bind descriptor heaps and tables
    BindDescriptorTables();
    
    // Render all primitives
    for (const auto& primitive : m_primitiveFactory->GetPrimitives())
    {
        // Set geometry buffers and topology for this primitive
        primitive->Set();
        // Set the pipeline state object (PSO)
        m_pso->Set();

        // Bind the vertex constant buffer for this primitive
        GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
            2,
            primitive->GetVertexConstantBuffer()->GetGPUHandle());

        // Bind the pixel constant buffer for this primitive
        GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
            3,
            primitive->GetPixelConstantBuffer()->GetGPUHandle());            

        // Issue the draw call for this primitive
        GRHI.GetCommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);
    }

    GUI.Render();

    // Prepare for present
    // Transition depth buffer to read state before presenting
    m_depthStencil->SetReadState();
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
// -----------------------------------------------------------------------------
void Renderer::OnUpdate()
{
    m_FrameInFlightIndex++;
    m_primitiveFactory->UpdateConstantBuffers();
    GUI.Update(0.0f);  
}

// -----------------------------------------------------------------------------
// Handles window resize events and recreates frame buffers
// -----------------------------------------------------------------------------
void Renderer::OnResize()
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
void Renderer::OnRender()
{
    // Wait for GPU to finish previous frame
    GRHI.WaitForGPU();

    GRHI.ResetCommandAllocator();
    GRHI.ResetCommandList();  

    // Update per-frame data
    OnUpdate();

    // Record rendering commands
    PopulateCommandList();

    GRHI.CloseCommandListScene();

    // Execute command list
    GRHI.ExecuteCommandList();

    // Signal fence for GPU completion
    GRHI.Signal();

    // Present the frame
    GSwapChain.Present();

    // Advance frame-in-flight index for next frame's per-frame resources
    GSwapChain.UpdateFrameInFlightIndex();
}

// -----------------------------------------------------------------------------
// Shuts down the renderer and all owned subsystems
// -----------------------------------------------------------------------------
void Renderer::Shutdown()
{
    GRHI.Flush();       
    GUI.Shutdown();    
    GSwapChain.Shutdown();
    GWindow.Shutdown();
    GDescriptorHeapManager.Shutdown();
    GRHI.Shutdown();
}