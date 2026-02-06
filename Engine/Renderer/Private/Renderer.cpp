#include "PCH.h"
#include "Renderer.h"
#include "Assets/AssetSystem.h"
#include "D3D12DebugLayer.h"
#include "D3D12Rhi.h"
#include "D3D12SwapChain.h"
#include "Window.h"
#include "DxcShaderCompiler.h"
#include "ShaderCompileResult.h"
#include "D3D12Texture.h"
#include "TextureManager.h"
#include "Renderer/Public/GPU/GPUMesh.h"
#include "Renderer/Public/GPU/GPUMeshCache.h"
#include "Scene/Scene.h"
#include "Scene/Mesh.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3D12RootBindings.h"
#include "D3D12ConstantBuffer.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12FrameResource.h"
#include "D3D12VertexLayout.h"
#include "Samplers/D3D12SamplerLibrary.h"
#include "D3D12DepthStencil.h"
#include "DepthConvention.h"
#include "UI.h"
#include "Time/Timer.h"
#include "RenderCamera.h"
#include "Scene/Camera/GameCamera.h"

Renderer::Renderer(Timer& timer, const AssetSystem& assetSystem, Scene& scene, Window& window) noexcept :
    m_timer(&timer), m_assetSystem(&assetSystem), m_scene(&scene), m_window(&window)
{
	// Create and own the RHI
	m_rhi = std::make_unique<D3D12Rhi>();

	// Open command list for initialization recording (starts closed after RHI creation)
	constexpr UINT kInitFrameIndex = 0;
	m_rhi->SetCurrentFrameIndex(kInitFrameIndex);
	m_rhi->ResetCommandAllocator(kInitFrameIndex);
	m_rhi->ResetCommandList(kInitFrameIndex);

	m_rootSignature = std::make_unique<D3D12RootSignature>(*m_rhi);

	// Compile shaders
	m_vertexShader = std::make_unique<ShaderCompileResult>(
	    DxcShaderCompiler::CompileFromAsset(*m_assetSystem, "Passes/Forward/ForwardLitVS.hlsl", ShaderStage::Vertex, "main"));
	m_pixelShader = std::make_unique<ShaderCompileResult>(
	    DxcShaderCompiler::CompileFromAsset(*m_assetSystem, "Passes/Forward/ForwardLitPS.hlsl", ShaderStage::Pixel, "main"));

	m_descriptorHeapManager = std::make_unique<D3D12DescriptorHeapManager>(*m_rhi);
	m_swapChain = std::make_unique<D3D12SwapChain>(*m_rhi, *m_window, *m_descriptorHeapManager);
	m_frameResourceManager = std::make_unique<D3D12FrameResourceManager>(*m_rhi, D3D12FrameResourceManager::DefaultCapacityPerFrame);

	// Create UI after descriptor heap manager is ready
	// UI subscribes to Window's OnWindowMessage event automatically in its constructor
	m_ui = std::make_unique<UI>(*m_timer, *m_rhi, *m_window, *m_descriptorHeapManager, *m_swapChain);

	m_constantBufferManager = std::make_unique<D3D12ConstantBufferManager>(
	    *m_timer,
	    *m_rhi,
	    *m_window,
	    *m_descriptorHeapManager,
	    *m_frameResourceManager,
	    *m_swapChain,
	    *m_ui);

	// Initialize sampler library first (requires contiguous descriptor allocation)
	m_samplerLibrary = std::make_unique<D3D12SamplerLibrary>(*m_rhi, *m_descriptorHeapManager);

	// Create texture manager (auto-loads default textures)
	m_textureManager = std::make_unique<TextureManager>(*m_assetSystem, *m_rhi, *m_descriptorHeapManager);

	// Create GPU mesh cache for lazy uploading CPU meshes
	m_gpuMeshCache = std::make_unique<GPUMeshCache>(*m_rhi);

	// Subscribe to events
	SubscribeToDepthModeChanges();
	SubscribeToWindowResize();

	// Create pipeline state object
	CreatePSO();

	CreateDepthStencilBuffer();

	// Create render camera bound to scene's game camera
	m_renderCamera = std::make_unique<RenderCamera>(m_scene->GetCamera());

	PostLoad();
}

void Renderer::PostLoad() noexcept
{
	// Execute initialization commands and sync with GPU
	m_rhi->CloseCommandList();
	m_rhi->ExecuteCommandList();
	m_rhi->Flush();
}

void Renderer::SetViewport() noexcept
{
	D3D12_VIEWPORT viewport = m_swapChain->GetDefaultViewport();
	m_rhi->GetCommandList()->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect = m_swapChain->GetDefaultScissorRect();
	m_rhi->GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void Renderer::SetBackBufferRTV() noexcept
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = m_swapChain->GetCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil->GetCPUHandle();
	m_rhi->GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
}

void Renderer::BindPerFrameResources() noexcept
{
	// Per-frame constant buffer (b0)
	m_rhi->GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerFrame,
	    m_constantBufferManager->GetPerFrameGpuAddress());

	// Per-view constant buffer (b1)
	m_rhi->GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerView,
	    m_constantBufferManager->GetPerViewGpuAddress());

	// Texture SRV descriptor table
	if (const D3D12Texture* checkerTex = m_textureManager->GetTexture(TextureId::Checker))
	{
		m_rhi->GetCommandList()->SetGraphicsRootDescriptorTable(RootBindings::RootParam::TextureSRV, checkerTex->GetGPUHandle());
	}

	// Sampler table (all samplers bound once per frame)
	if (m_samplerLibrary && m_samplerLibrary->IsInitialized())
	{
		m_rhi->GetCommandList()->SetGraphicsRootDescriptorTable(
		    RootBindings::RootParam::SamplerTable,
		    m_samplerLibrary->GetTableGPUHandle());
	}
}

void Renderer::BindPerObjectResources(const Mesh& mesh) noexcept
{
	// Build per-object VS constant buffer data from mesh transforms
	PerObjectVSConstantBufferData perObjectData{};
	DirectX::XMStoreFloat4x4(&perObjectData.WorldMTX, mesh.GetWorldMatrix());

	// Store inverse-transpose as 3x4 for normal transformation (matches HLSL cbuffer float3x3 packing)
	const DirectX::XMMATRIX worldInvTranspose = mesh.GetWorldInverseTransposeMatrix();
	DirectX::XMStoreFloat3x4(&perObjectData.WorldInvTransposeMTX, worldInvTranspose);

	// Per-object VS constant buffer (b2) - world matrix
	m_rhi->GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerObjectVS,
	    m_constantBufferManager->UpdatePerObjectVS(perObjectData));

	// Per-object PS constant buffer (b3) - material properties
	m_rhi->GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerObjectPS,
	    m_constantBufferManager->UpdatePerObjectPS());
}

void Renderer::PopulateCommandList()
{
	// Prepare render target and depth buffer
	m_swapChain->SetRenderTargetState();
	m_depthStencil->SetWriteState();

	// Set root signature and render state
	m_rhi->GetCommandList()->SetGraphicsRootSignature(m_rootSignature->GetRaw());
	SetViewport();
	SetBackBufferRTV();

	// Clear targets
	m_swapChain->Clear();
	m_depthStencil->Clear();

	// Set shader-visible descriptor heaps
	m_descriptorHeapManager->SetShaderVisibleHeaps();

	// Bind per-frame resources
	BindPerFrameResources();

	// Draw loop
	m_pso->Set();

	const auto& meshes = m_scene->GetMeshes();
	for (const auto& mesh : meshes)
	{
		GPUMesh* gpuMesh = m_gpuMeshCache->GetOrUpload(*mesh);
		gpuMesh->Bind(m_rhi->GetCommandList().Get());
		BindPerObjectResources(*mesh);
		m_rhi->GetCommandList()->DrawIndexedInstanced(gpuMesh->GetIndexCount(), 1, 0, 0, 0);
	}

	// Post-render
	m_ui->Render();
	m_depthStencil->SetReadState();
	m_swapChain->SetPresentState();
}

void Renderer::CreateDepthStencilBuffer()
{
	m_depthStencil = std::make_unique<D3D12DepthStencil>(*m_rhi, *m_window, *m_descriptorHeapManager);
}

void Renderer::OnResize() noexcept
{
	m_rhi->Flush();
	m_swapChain->Resize();
	CreateDepthStencilBuffer();
}

void Renderer::SubscribeToDepthModeChanges() noexcept
{
	auto handle = DepthConvention::OnModeChanged.Add(
	    [this](DepthMode mode)
	    {
		    OnDepthModeChanged(mode);
	    });
	m_depthModeChangedHandle = ScopedEventHandle(DepthConvention::OnModeChanged, handle);
}

void Renderer::SubscribeToWindowResize() noexcept
{
	auto handle = m_window->OnResized.Add(
	    [this]()
	    {
		    OnResize();
	    });
	m_resizeHandle = ScopedEventHandle(m_window->OnResized, handle);
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
	const UINT frameIndex = m_swapChain->GetFrameInFlightIndex();
	m_rhi->SetCurrentFrameIndex(frameIndex);
	m_frameResourceManager->BeginFrame(m_rhi->GetFence().Get(), m_rhi->GetFenceEvent(), frameIndex);
	m_rhi->WaitForGPU(frameIndex);
	m_rhi->ResetCommandAllocator(frameIndex);
	m_rhi->ResetCommandList(frameIndex);
}

void Renderer::SetupFrame() noexcept
{
	m_renderCamera->Update();

	m_timer->Tick();
	m_ui->Update();
	m_constantBufferManager->UpdatePerFrame();
	m_constantBufferManager->UpdatePerView(*m_renderCamera);
}

void Renderer::RecordFrame() noexcept
{
	PopulateCommandList();
}

void Renderer::SubmitFrame() noexcept
{
	m_rhi->CloseCommandList();
	m_rhi->ExecuteCommandList();
	m_rhi->Signal(m_swapChain->GetFrameInFlightIndex());

	// Record fence value for ring buffer synchronization
	m_frameResourceManager->EndFrame(m_rhi->GetNextFenceValue() - 1);
	m_swapChain->Present();
}

void Renderer::EndFrame() noexcept
{
	m_swapChain->UpdateFrameInFlightIndex();
}

// -----------------------------------------------------------------------------
// Shuts down the renderer and all owned subsystems
// -----------------------------------------------------------------------------
Renderer::~Renderer() noexcept
{
	m_rhi->Flush();

	m_renderCamera.reset();

	m_pso.reset();
	m_rootSignature.reset();
	m_scene = nullptr;
	m_depthStencil.reset();
	m_samplerLibrary.reset();
	m_textureManager.reset();

	m_constantBufferManager.reset();
	m_frameResourceManager.reset();
	m_swapChain.reset();
	m_ui.reset();
	m_descriptorHeapManager.reset();
}

void Renderer::CreatePSO()
{
	m_pso = std::make_unique<D3D12PipelineState>(
	    *m_rhi,
	    D3D12VertexLayout::GetStaticMeshLayout(),
	    *m_rootSignature,
	    m_vertexShader->GetBytecode(),
	    m_pixelShader->GetBytecode());
}

void Renderer::OnDepthModeChanged([[maybe_unused]] DepthMode mode) noexcept
{
	// Depth convention changed - must recreate PSO with new depth comparison
	// and depth stencil buffer with new optimized clear value
	m_rhi->Flush();
	CreatePSO();
	CreateDepthStencilBuffer();
}
