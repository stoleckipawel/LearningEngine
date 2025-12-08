
#include "PCH.h"
#include "Renderer.h"
#include "D3D12/DebugLayer.h"
#include "D3D12/RHI.h"
#include "Window.h"
#include "D3D12/ConstantBufferManager.h"
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
    GConstantBufferManager.Initialize();
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
    m_primitiveFactory->AppendBox();
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

    // Vertex constant buffer
    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        2,
        GConstantBufferManager.VertexConstantBuffers[GSwapChain.GetFrameInFlightIndex()]->GetGPUHandle());

    // Pixel constant buffer
    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        3,
        GConstantBufferManager.PixelConstantBuffers[GSwapChain.GetFrameInFlightIndex()]->GetGPUHandle());
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
    m_primitiveFactory->GetFirstPrimitive().Set();

    // Bind descriptor heaps and tables
    GDescriptorHeapManager.SetShaderVisibleHeaps();
    BindDescriptorTables();
    m_pso->Set();

    // Draw geometry (hardcoded cube: 36 indices)
    GRHI.GetCommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);

    
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
    GConstantBufferManager.Update(m_FrameInFlightIndex);
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