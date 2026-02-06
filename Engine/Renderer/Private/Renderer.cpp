#include "PCH.h"
#include "Renderer.h"
#include "Assets/AssetSystem.h"
#include "D3D12DebugLayer.h"
#include "D3D12Rhi.h"
#include "D3D12SwapChain.h"
#include "Window.h"
#include "DxcShaderCompiler.h"
#include "ShaderCompileResult.h"
#include "TextureManager.h"
#include "Renderer/Public/GPU/GPUMeshCache.h"
#include "Scene/Scene.h"
#include "Scene/Mesh.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3D12ConstantBuffer.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12ConstantBufferData.h"
#include "D3D12FrameResource.h"
#include "D3D12VertexLayout.h"
#include "Samplers/D3D12SamplerLibrary.h"
#include "D3D12DepthStencil.h"
#include "DepthConvention.h"
#include "UI.h"
#include "Time/Timer.h"
#include "Renderer/Public/Camera/RenderCamera.h"
#include "Renderer/Public/RenderContext.h"
#include "Renderer/Public/FrameGraph/FrameGraph.h"
#include "Renderer/Public/Passes/ForwardOpaquePass.h"
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

	// Create Frame Graph and register passes
	m_frameGraph = std::make_unique<FrameGraph>(m_swapChain.get(), m_depthStencil.get());
	m_frameGraph->AddPass<ForwardOpaquePass>(
	    "ForwardOpaque",
	    *m_rootSignature,
	    *m_pso,
	    *m_constantBufferManager,
	    *m_descriptorHeapManager,
	    *m_textureManager,
	    *m_samplerLibrary,
	    *m_gpuMeshCache,
	    *m_swapChain,
	    *m_depthStencil);

	PostLoad();
}

void Renderer::PostLoad() noexcept
{
	// Execute initialization commands and sync with GPU
	m_rhi->CloseCommandList();
	m_rhi->ExecuteCommandList();
	m_rhi->Flush();
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
}

void Renderer::RecordFrame() noexcept
{
	// Build scene view from current frame state
	SceneView sceneView = BuildSceneView();

	// Build per-view constant buffer data (camera + sun light)
	PerViewConstantBufferData viewData = m_renderCamera->GetViewConstantBufferData();
	viewData.SunDirection = sceneView.sunLight.direction;
	viewData.SunIntensity = sceneView.sunLight.intensity;
	viewData.SunColor = sceneView.sunLight.color;
	m_constantBufferManager->UpdatePerView(viewData);

	// Frame graph: declare resource usage
	m_frameGraph->Setup(sceneView);

	// Frame graph: compile (MVP: no-op)
	m_frameGraph->Compile();

	// Create render context for this frame's command list
	RenderContext context(m_rhi->GetCommandList().Get());

	// Frame graph: record all pass commands
	m_frameGraph->Execute(context);

	// UI overlay (after all passes, before present transition)
	m_ui->Render();

	// Transition resources for presentation
	m_depthStencil->SetReadState();
	m_swapChain->SetPresentState();
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
// Scene View — per-frame data preparation
// -----------------------------------------------------------------------------

SceneView Renderer::BuildSceneView() const
{
	SceneView view = {};

	// Viewport (from window, which swap chain tracks)
	view.width = m_window->GetWidth();
	view.height = m_window->GetHeight();

	// Camera — store pointer to already-updated RenderCamera
	view.camera = m_renderCamera.get();

	// Lighting — struct defaults (sun down, white, intensity 1)
	// Materials — single default PBR material at index 0
	view.materials.emplace_back();

	// Draw commands
	BuildMeshDraws(view);

	return view;
}

void Renderer::BuildMeshDraws(SceneView& view) const
{
	if (!m_scene || !m_scene->HasMeshes())
		return;

	const auto& meshes = m_scene->GetMeshes();
	view.meshDraws.reserve(meshes.size());

	for (const auto& mesh : meshes)
	{
		MeshDraw draw = {};
		DirectX::XMStoreFloat4x4(&draw.worldMatrix, mesh->GetWorldMatrix());
		DirectX::XMStoreFloat3x4(&draw.worldInvTranspose, mesh->GetWorldInverseTransposeMatrix());
		draw.materialId = mesh->GetMaterialId();
		draw.meshPtr = mesh.get();
		view.meshDraws.push_back(draw);
	}
}

// -----------------------------------------------------------------------------
// Shuts down the renderer and all owned subsystems
// -----------------------------------------------------------------------------
Renderer::~Renderer() noexcept
{
	m_rhi->Flush();


	m_frameGraph.reset();

	m_renderCamera.reset();

	m_pso.reset();
	m_rootSignature.reset();
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
