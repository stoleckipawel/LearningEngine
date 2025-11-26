#include "Renderer.h"
#include "DebugLayer.h"
#include "RHI.h"
#include "Window.h"
#include "ConstantBufferManager.h"

// Global renderer instance
Renderer GRenderer;

// Uploads geometry data to the GPU
void Renderer::LoadGeometry()
{
    m_vertecies = std::make_unique<Geometry>();
}

// Loads texture resources
void Renderer::LoadTextures()
{
    m_texture = std::make_unique<Texture>("Assets/Textures/Test1.png", 0);
}

// Initializes the sampler state
void Renderer::LoadSamplers()
{
    m_sampler = std::make_unique<Sampler>(0);
}

// Compiles and loads vertex and pixel shaders
void Renderer::LoadShaders()
{
    m_vertexShader = std::make_unique<ShaderCompiler>(L"Shaders/VertShader.hlsl", "vs_5_0", "main"); // TODO: Check shader model 6.0 support
    m_pixelShader = std::make_unique<ShaderCompiler>(L"Shaders/PixShader.hlsl", "ps_5_0", "main");
}

// Creates the root signature for the pipeline
void Renderer::CreateRootSignatures()
{
    m_rootSignature = std::make_unique<RootSignature>();
}

// Creates the pipeline state object (PSO)
void Renderer::CreatePSOs()
{
    m_pso = std::make_unique<PSO>(*m_vertecies, m_rootSignature->Get(), *m_vertexShader, *m_pixelShader);
}

// Finalizes resource uploads and flushes the command queue
void Renderer::PostLoad()
{
    GRHI.CloseCommandLists();
    GRHI.ExecuteCommandList();
    GRHI.Flush();
}

// Loads all resources and initializes the rendering pipeline
void Renderer::Load()
{
    CreateRootSignatures();
    LoadGeometry();
    LoadShaders();
    GDescriptorHeapManager.Initialize();
    GSwapChain.Initialize();
    GConstantBufferManager.Initialize();
    LoadTextures();
    LoadSamplers();
    CreatePSOs();
    CreateFrameBuffers();
    PostLoad();
}

// Releases all resources and subsystems
void Renderer::Release()
{
    GDescriptorHeapManager.Release();
}

// Sets the viewport and scissor rectangle for rasterization
void Renderer::SetViewport()
{
    D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
    GRHI.GetCommandList()->RSSetViewports(1, &viewport);

    D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
    GRHI.GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

// Sets the render target and depth stencil views
void Renderer::SetBackBufferRTV()
{
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetCPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil->GetCPUHandle();
    GRHI.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
}

// Binds descriptor tables for textures, samplers, and constant buffers
void Renderer::BindDescriptorTables()
{
    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        0,
        m_texture->GetGPUHandle());

    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        1,
        m_sampler->GetGPUHandle());

    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        2,
        GConstantBufferManager.VertexConstantBuffers[GSwapChain.GetBackBufferIndex()]->GetGPUHandle());

    GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
        3,
        GConstantBufferManager.PixelConstantBuffers[GSwapChain.GetBackBufferIndex()]->GetGPUHandle());
}

// Records all rendering commands for the current frame
void Renderer::PopulateCommandList()
{
    GSwapChain.SetRenderTargetState();

    GRHI.GetCommandList()->SetGraphicsRootSignature(m_rootSignature->Get());

    SetViewport();
    SetBackBufferRTV();
    GSwapChain.Clear();

    m_depthStencil->Clear();
    m_vertecies->Set();

    GDescriptorHeapManager.SetShaderVisibleHeaps();
    BindDescriptorTables();
    m_pso->Set();

    GRHI.GetCommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);

    GSwapChain.SetPresentState();
}

// Creates frame buffers and depth stencil resources
void Renderer::CreateFrameBuffers()
{
    m_depthStencil = std::make_unique<DepthStencil>(0);
}

// Updates per-frame data and constant buffers
void Renderer::OnUpdate()
{
    m_frameIndex++;
    GConstantBufferManager.Update(m_frameIndex);
}

// Handles window resize events and recreates frame buffers
void Renderer::OnResize()
{
    GRHI.Flush();
    CreateFrameBuffers();
}

// Main render loop for the scene
void Renderer::OnRender()
{
    OnUpdate();

    GRHI.WaitForGPU();

    // Reset command allocator and command list for new frame
    ThrowIfFailed(GRHI.GetCommandAllocator()->Reset(), "Renderer: Failed To Reset Command Allocator");
    ThrowIfFailed(GRHI.GetCommandList()->Reset(GRHI.GetCommandAllocator().Get(), m_pso->Get().Get()), "Renderer: Failed To Reset Command List");

    // Record rendering commands
    PopulateCommandList();

    // Close command list
    ThrowIfFailed(GRHI.GetCommandList()->Close(), "Failed To Close Command List");

    // Execute command list
    GRHI.ExecuteCommandList();

    // Signal fence for GPU completion
    GRHI.Signal();

    // Present the frame
    GSwapChain.Present();

    // Update back buffer index
    GSwapChain.UpdateCurrentBackBufferIndex();
}

// Shuts down the renderer and all subsystems
void Renderer::Shutdown()
{
    GRHI.Flush();

    Renderer::Release();
    GSwapChain.Shutdown();
    GWindow.Shutdown();
    GRHI.Shutdown();
    GDebugLayer.Shutdown();
}