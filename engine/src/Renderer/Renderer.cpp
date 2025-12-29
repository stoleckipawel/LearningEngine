#include "PCH.h"
#include "Renderer.h"
#include "D3D12DebugLayer.h"
#include "D3D12Rhi.h"
#include "Window.h"
#include "DxcShaderCompiler.h"
#include "Texture.h"
#include "Scene/Scene.h"
#include "Scene/Mesh.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3D12RootBindings.h"
#include "D3D12ConstantBuffer.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12FrameResource.h"
#include "Samplers/D3D12SamplerLibrary.h"
#include "D3D12DepthStencil.h"
#include "UI.h"
#include "Timer.h"
#include "Camera.h"
#include "Log.h"

Renderer GRenderer;

void Renderer::Initialize() noexcept
{
	GD3D12Rhi.Initialize();

	m_rootSignature = std::make_unique<D3D12RootSignature>();

	// Compile shaders
	m_vertexShader = DxcShaderCompiler::CompileFromAsset("Passes/Forward/ForwardLitVS.hlsl", ShaderStage::Vertex, "main");
	m_pixelShader = DxcShaderCompiler::CompileFromAsset("Passes/Forward/ForwardLitPS.hlsl", ShaderStage::Pixel, "main");

	GD3D12DescriptorHeapManager.Initialize();
	GD3D12SwapChain.Initialize();
	GD3D12FrameResourceManager.Initialize(D3D12FrameResourceManager::DefaultCapacityPerFrame);
	GD3D12ConstantBufferManager.Initialize();

	// Initialize sampler library first (requires contiguous descriptor allocation)
	m_samplerLibrary = std::make_unique<D3D12SamplerLibrary>();
	m_samplerLibrary->Initialize();

	// Load textures
	m_checkerTexture = std::make_unique<Texture>(std::filesystem::path("ColorCheckerBoard.png"));
	m_skyCubemapTexture = std::make_unique<Texture>(std::filesystem::path("SkyCubemap.png"));

	// Initialize scene (geometry is built when UI triggers SetPrimitives)
	GScene.Initialize();

	// Create pipeline state object
	m_pso = std::make_unique<D3D12PipelineState>(
	    Mesh::GetStaticVertexLayout(),
	    *m_rootSignature,
	    m_vertexShader.GetBytecode(),
	    m_pixelShader.GetBytecode());

	// Create depth stencil and other frame buffers
	CreateFrameBuffers();

	GUI.Initialize();

	PostLoad();
}

void Renderer::PostLoad() noexcept
{
	GD3D12Rhi.CloseCommandListScene();
	GD3D12Rhi.ExecuteCommandList();
	GD3D12Rhi.Flush();
}

void Renderer::SetViewport() noexcept
{
	D3D12_VIEWPORT viewport = GD3D12SwapChain.GetDefaultViewport();
	GD3D12Rhi.GetCommandList()->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect = GD3D12SwapChain.GetDefaultScissorRect();
	GD3D12Rhi.GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void Renderer::SetBackBufferRTV() noexcept
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GD3D12SwapChain.GetCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil->GetCPUHandle();
	GD3D12Rhi.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
}

void Renderer::BindPerFrameResources() noexcept
{
	// Per-frame constant buffer (b0)
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerFrame,
	    GD3D12ConstantBufferManager.GetPerFrameGpuAddress());

	// Per-view constant buffer (b1)
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerView,
	    GD3D12ConstantBufferManager.GetPerViewGpuAddress());

	// Texture SRV descriptor table
	if (m_checkerTexture)
	{
		GD3D12Rhi.GetCommandList()->SetGraphicsRootDescriptorTable(RootBindings::RootParam::TextureSRV, m_checkerTexture->GetGPUHandle());
	}

	// Sampler table (all samplers bound once per frame)
	if (m_samplerLibrary && m_samplerLibrary->IsInitialized())
	{
		GD3D12Rhi.GetCommandList()->SetGraphicsRootDescriptorTable(
		    RootBindings::RootParam::SamplerTable,
		    m_samplerLibrary->GetTableGPUHandle());
	}
}

void Renderer::BindPerObjectResources(const Mesh& mesh) noexcept
{
	// Per-object VS constant buffer (b2) - world matrix
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerObjectVS,
	    GD3D12ConstantBufferManager.UpdatePerObjectVS(mesh.GetPerObjectVSConstants()));

	// Per-object PS constant buffer (b3) - material properties
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerObjectPS,
	    GD3D12ConstantBufferManager.UpdatePerObjectPS());
}

void Renderer::PopulateCommandList()
{
	// Prepare render target and depth buffer
	GD3D12SwapChain.SetRenderTargetState();
	m_depthStencil->SetWriteState();

	// Set root signature and render state
	GD3D12Rhi.GetCommandList()->SetGraphicsRootSignature(m_rootSignature->GetRaw());
	SetViewport();
	SetBackBufferRTV();

	// Clear targets
	GD3D12SwapChain.Clear();
	m_depthStencil->Clear();

	// Set shader-visible descriptor heaps
	GD3D12DescriptorHeapManager.SetShaderVisibleHeaps();

	// Bind per-frame resources
	BindPerFrameResources();

	// Draw loop
	m_pso->Set();

	const auto& meshes = GScene.GetMeshes();
	for (const auto& mesh : meshes)
	{
		mesh->Bind(GD3D12Rhi.GetCommandList().Get());
		BindPerObjectResources(*mesh);
		GD3D12Rhi.GetCommandList()->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
	}

	// Post-render
	GUI.Render();
	m_depthStencil->SetReadState();
	GD3D12SwapChain.SetPresentState();
}

void Renderer::CreateFrameBuffers()
{
	m_depthStencil = std::make_unique<D3D12DepthStencil>();
}

void Renderer::OnResize() noexcept
{
	GD3D12Rhi.Flush();
	GD3D12SwapChain.Resize();
	CreateFrameBuffers();
}

void Renderer::OnRender() noexcept
{
	BeginFrame();
	SetupFrame();
	RecordFrame();
	SubmitFrame();
	EndFrame();
}

void Renderer::BeginFrame() noexcept
{
	GD3D12FrameResourceManager.BeginFrame(GD3D12Rhi.GetFenceEvent(), GD3D12SwapChain.GetFrameInFlightIndex());
	GD3D12Rhi.WaitForGPU();
	GD3D12Rhi.ResetCommandAllocator();
	GD3D12Rhi.ResetCommandList();
}

void Renderer::SetupFrame() noexcept
{
	GTimer.Tick();
	GUI.Update();
	GD3D12ConstantBufferManager.UpdatePerFrame();
	GD3D12ConstantBufferManager.UpdatePerView();
}

void Renderer::RecordFrame() noexcept
{
	PopulateCommandList();
}

void Renderer::SubmitFrame() noexcept
{
	GD3D12Rhi.CloseCommandListScene();
	GD3D12Rhi.ExecuteCommandList();
	GD3D12Rhi.Signal();

	// Record fence value for ring buffer synchronization
	GD3D12FrameResourceManager.EndFrame(GD3D12Rhi.GetNextFenceValue() - 1);
	GD3D12SwapChain.Present();
}

void Renderer::EndFrame() noexcept
{
	GD3D12SwapChain.UpdateFrameInFlightIndex();
}

// -----------------------------------------------------------------------------
// Shuts down the renderer and all owned subsystems
// -----------------------------------------------------------------------------
void Renderer::Shutdown() noexcept
{
	GD3D12Rhi.Flush();

	GUI.Shutdown();

	m_pso.reset();
	m_rootSignature.reset();
	GScene.Shutdown();
	m_depthStencil.reset();
	m_samplerLibrary.reset();
	m_skyCubemapTexture.reset();
	m_checkerTexture.reset();

	GD3D12ConstantBufferManager.Shutdown();
	GD3D12FrameResourceManager.Shutdown();
	GD3D12SwapChain.Shutdown();
	GWindow.Shutdown();
	GD3D12DescriptorHeapManager.Shutdown();
	GD3D12Rhi.Shutdown();
}