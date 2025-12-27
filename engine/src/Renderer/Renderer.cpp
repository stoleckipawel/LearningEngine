#include "PCH.h"
#include "Renderer.h"
#include "D3D12DebugLayer.h"
#include "D3D12Rhi.h"
#include "Window.h"
#include "DxcShaderCompiler.h"
#include "Texture.h"
#include "PrimitiveFactory.h"
#include "D3D12PipelineState.h"
#include "D3D12RootSignature.h"
#include "D3D12RootBindings.h"
#include "D3D12ConstantBuffer.h"
#include "D3D12ConstantBufferManager.h"
#include "D3D12FrameResource.h"
#include "D3D12Sampler.h"
#include "D3D12DepthStencil.h"
#include "UI.h"
#include "Timer.h"

// Global renderer instance
Renderer GRenderer;

// -----------------------------------------------------------------------------
// Initializes all graphics subsystems and resources
// -----------------------------------------------------------------------------
void Renderer::Initialize() noexcept
{
	// Initialize the rendering hardware interface (RHI)
	GD3D12Rhi.Initialize();

	// Create root signature first - defines shader resource binding layout
	m_rootSignature = std::make_unique<D3D12RootSignature>();

	m_vertexShader = std::make_unique<DxcShaderCompiler>("SimpleVS.hlsl", DxcShaderCompiler::ShaderStage::Vertex, "main");
	m_pixelShader = std::make_unique<DxcShaderCompiler>("SimplePS.hlsl", DxcShaderCompiler::ShaderStage::Pixel, "main");

	// Initialize descriptor heap manager and swap chain
	GD3D12DescriptorHeapManager.Initialize();
	GD3D12SwapChain.Initialize();

	// Initialize frame resource manager (per-frame ring buffer for dynamic CBs)
	GD3D12FrameResourceManager.Initialize(D3D12FrameResourceManager::DefaultCapacityPerFrame);

	// Initialize the global constant buffer manager (manages per-frame CB instances)
	GD3D12ConstantBufferManager.Initialize();

	// Load textures and create sampler
	m_checkerTexture = std::make_unique<Texture>(std::filesystem::path("ColorCheckerBoard.png"));
	m_skyCubemapTexture = std::make_unique<Texture>(std::filesystem::path("SkyCubemap.png"));
	m_sampler = std::make_unique<D3D12Sampler>();

	// Create geometry
	GatherPrimitives();

	// Create pipeline state object
	m_pso = std::make_unique<D3D12PipelineState>(Primitive::GetStaticVertexLayout(), *m_rootSignature, *m_vertexShader, *m_pixelShader);

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

	// Hard-coded 20 primitive with varied translation, rotation, and smaller scale or further from camera
	std::vector<std::tuple<XMFLOAT3, XMFLOAT3, XMFLOAT3>> shapeParams = {
	    // translation                rotation (radians)           scale
		{{-10.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {1.2f, 1.2f, 1.2f}}, {{-8.0f, 2.0f, 6.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
		{{-6.0f, -2.0f, -8.0f}, {0.5f, 0.5f, 0.0f}, {1.5f, 1.5f, 1.5f}}, {{-4.0f, 0.0f, 8.0f}, {0.0f, 0.7f, 0.0f}, {1.3f, 1.3f, 1.3f}},
		{{-2.0f, 2.0f, -6.0f}, {1.0f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}},  {{0.0f, -2.0f, 6.0f}, {0.0f, 0.0f, 1.0f}, {0.9f, 0.9f, 0.9f}},
		{{2.0f, 0.0f, -8.0f}, {0.3f, 0.8f, 0.2f}, {1.2f, 1.2f, 1.2f}},   {{4.0f, 4.0f, 8.0f}, {0.0f, 1.2f, 0.0f}, {1.0f, 1.0f, 1.0f}},
		{{6.0f, -4.0f, -6.0f}, {1.0f, 0.5f, 0.0f}, {0.7f, 0.7f, 0.7f}},  {{8.0f, 0.0f, 6.0f}, {0.7f, 0.0f, 1.0f}, {1.3f, 1.3f, 1.3f}},
		{{-9.0f, -3.0f, 10.0f}, {0.2f, 0.3f, 0.4f}, {0.8f, 0.8f, 0.8f}}, {{-7.0f, 3.0f, -10.0f}, {0.6f, 0.1f, 0.2f}, {1.1f, 1.1f, 1.1f}},
		{{-5.0f, -1.0f, 9.0f}, {0.4f, 0.6f, 0.8f}, {1.0f, 1.0f, 1.0f}},  {{-3.0f, 1.0f, -9.0f}, {0.9f, 0.2f, 0.3f}, {0.9f, 0.9f, 0.9f}},
		{{-1.0f, -3.0f, 8.0f}, {0.1f, 0.4f, 0.7f}, {1.2f, 1.2f, 1.2f}},  {{1.0f, 3.0f, -8.0f}, {0.5f, 0.9f, 0.1f}, {1.0f, 1.0f, 1.0f}},
		{{3.0f, -1.0f, 7.0f}, {0.8f, 0.3f, 0.6f}, {0.7f, 0.7f, 0.7f}},   {{5.0f, 1.0f, -7.0f}, {0.2f, 0.7f, 0.5f}, {1.1f, 1.1f, 1.1f}},
		{{7.0f, -3.0f, 6.0f}, {0.3f, 0.6f, 0.9f}, {0.8f, 0.8f, 0.8f}},   {{9.0f, 3.0f, -5.0f}, {0.7f, 0.2f, 0.4f}, {1.0f, 1.0f, 1.0f}}};

	for (const auto& [translation, rotation, scale] : shapeParams)
	{
		m_primitiveFactory->AppendShape(PrimitiveFactory::Shape::Sphere, translation, rotation, scale);
	}

	m_primitiveFactory->Upload();
}

// -----------------------------------------------------------------------------
// Finalizes resource uploads and flushes the command queue
// -----------------------------------------------------------------------------
void Renderer::PostLoad() noexcept
{
	GD3D12Rhi.CloseCommandListScene();
	GD3D12Rhi.ExecuteCommandList();
	GD3D12Rhi.Flush();
}

// -----------------------------------------------------------------------------
// Sets viewport and scissor rectangle for rasterization
// -----------------------------------------------------------------------------
void Renderer::SetViewport() noexcept
{
	D3D12_VIEWPORT viewport = GD3D12SwapChain.GetDefaultViewport();
	GD3D12Rhi.GetCommandList()->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect = GD3D12SwapChain.GetDefaultScissorRect();
	GD3D12Rhi.GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

// -----------------------------------------------------------------------------
// Sets render target and depth stencil views for output merger
// -----------------------------------------------------------------------------
void Renderer::SetBackBufferRTV() noexcept
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GD3D12SwapChain.GetCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil->GetCPUHandle();
	GD3D12Rhi.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
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
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerFrame,
	    GD3D12ConstantBufferManager.GetPerFrameGpuAddress());

	// -------------------------------------------------------------------------
	// Bind Per-View Constant Buffer - updated once per view/camera
	// Contains: View/Proj matrices, CameraPosition, Near/Far planes.............
	// -------------------------------------------------------------------------
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerView,
	    GD3D12ConstantBufferManager.GetPerViewGpuAddress());

	// -------------------------------------------------------------------------
	// Bind Textures SRV - descriptor table
	// -------------------------------------------------------------------------
	if (m_checkerTexture)
	{
		GD3D12Rhi.GetCommandList()->SetGraphicsRootDescriptorTable(RootBindings::RootParam::TextureSRV, m_checkerTexture->GetGPUHandle());
	}

	// -------------------------------------------------------------------------
	// Bind Samplers - descriptor table
	// -------------------------------------------------------------------------
	if (m_sampler)
	{
		GD3D12Rhi.GetCommandList()->SetGraphicsRootDescriptorTable(RootBindings::RootParam::Sampler, m_sampler->GetGPUHandle());
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
	// Uses ring buffer allocation - each call returns a unique GPU VA
	// -------------------------------------------------------------------------
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerObjectVS,
	    GD3D12ConstantBufferManager.UpdatePerObjectVS(primitive));

	// -------------------------------------------------------------------------
	// Update and Bind Per-Object PS Constant Buffer (b3)
	// Contains: Material properties (BaseColor, Metallic, Roughness, F0)
	// Uses ring buffer allocation - each call returns a unique GPU VA
	// TODO: Pass material data from primitive when material system is implemented
	// -------------------------------------------------------------------------
	GD3D12Rhi.GetCommandList()->SetGraphicsRootConstantBufferView(
	    RootBindings::RootParam::PerObjectPS,
	    GD3D12ConstantBufferManager.UpdatePerObjectPS());
}

// -----------------------------------------------------------------------------
// Records all rendering commands for the current frame
// -----------------------------------------------------------------------------
void Renderer::PopulateCommandList()
{
	// -----------------------------------------------------------------------------
	// FRAME SETUP - Execute once per frame
	// -----------------------------------------------------------------------------

	// Prepare render target - transition to render target state
	GD3D12SwapChain.SetRenderTargetState();

	// Transition depth buffer to write state before rendering
	m_depthStencil->SetWriteState();

	// Bind root signature - defines the shader resource layout
	GD3D12Rhi.GetCommandList()->SetGraphicsRootSignature(m_rootSignature->GetRaw());

	// Set viewport and scissor rect
	SetViewport();

	// Set render targets (back buffer + depth stencil)
	SetBackBufferRTV();

	// Clear render targets
	GD3D12SwapChain.Clear();
	m_depthStencil->Clear();

	// Set shader-visible descriptor heaps
	GD3D12DescriptorHeapManager.SetShaderVisibleHeaps();

	// -----------------------------------------------------------------------------
	// PER-FRAME BINDINGS - Resources that don't change during the frame
	// -----------------------------------------------------------------------------
	BindPerFrameResources();

	// -----------------------------------------------------------------------------
	// DRAW LOOP - Per-object bindings and draw calls
	// -----------------------------------------------------------------------------

	// Set the pipeline state object (PSO)
	// ToDo: sort by PSO to minimize state changes
	m_pso->Set();	
	
	const auto& primitives = m_primitiveFactory->GetPrimitives();

	for (const auto& primitive : primitives)
	{
		// Set geometry buffers (VB, IB) and topology for this primitive
		primitive->Set();

		// Bind per-object constant buffers (world matrix, material)
		BindPerObjectResources(*primitive);

		// Issue the draw call for this primitive
		GD3D12Rhi.GetCommandList()->DrawIndexedInstanced(primitive->GetIndexCount(), 1, 0, 0, 0);
	}

	// -----------------------------------------------------------------------------
	// POST-RENDER
	// -----------------------------------------------------------------------------

#if USE_GUI
	GUI.Render();
#endif

	// Transition depth buffer to read state before presenting
	m_depthStencil->SetReadState();

	// Transition back buffer to present state
	GD3D12SwapChain.SetPresentState();
}

// -----------------------------------------------------------------------------
// Creates frame buffers and depth stencil resources
// -----------------------------------------------------------------------------
void Renderer::CreateFrameBuffers()
{
	m_depthStencil = std::make_unique<D3D12DepthStencil>();
}

// -----------------------------------------------------------------------------
// Handles window resize events and recreates frame buffers
// -----------------------------------------------------------------------------
void Renderer::OnResize() noexcept
{
	GD3D12Rhi.Flush();
	// Resize swap chain to match new window dimensions
	GD3D12SwapChain.Resize();
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
	// Wait for GPU to finish with this frame's resources before reusing: This is where the ring buffer synchronization
	// happens
	GD3D12FrameResourceManager.BeginFrame(GD3D12Rhi.GetFenceEvent(), GD3D12SwapChain.GetFrameInFlightIndex());

	GD3D12Rhi.WaitForGPU();
	GD3D12Rhi.ResetCommandAllocator();
	GD3D12Rhi.ResetCommandList();
}

// -----------------------------------------------------------------------------
// SetupFrame: perform per-frame CPU updates (timing, camera, per-frame CBs)
// This is where heavy CPU work and culling would occur in a full engine.
// -----------------------------------------------------------------------------
void Renderer::SetupFrame() noexcept
{
	// -----------------------------------------------------------------------------
	// TIMING - Update global time source first (other systems depend on it)
	// -----------------------------------------------------------------------------
	GTimer.Tick();

	// -----------------------------------------------------------------------------
	// CONSTANT BUFFER UPDATES - Update CB data at appropriate frequencies
	// -----------------------------------------------------------------------------

	// Per-Frame CB (b0): Updated once per CPU frame
	// Contains: FrameIndex, TotalTime, DeltaTime, ViewportSize
	GD3D12ConstantBufferManager.UpdatePerFrame();

	// Per-View CB (b1): Updated once per camera/view
	// Contains: View/Proj matrices, CameraPosition, Near/Far
	// ToDo: Camera matrices are lazily rebuilt when accessed via GCamera.
	//       In a multi-view scenario (shadows, reflections), UpdatePerView()
	//       would be called once per view, not once per frame.
	GD3D12ConstantBufferManager.UpdatePerView();

#if USE_GUI
	// Pass seconds to UI which expects seconds-precision delta
	GUI.Update();
#endif
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
	GD3D12Rhi.CloseCommandListScene();
	GD3D12Rhi.ExecuteCommandList();
	GD3D12Rhi.Signal();

	// Record the fence value for this frame so we know when GPU is done
	// This enables safe ring buffer reuse after FramesInFlight frames
	GD3D12FrameResourceManager.EndFrame(GD3D12Rhi.GetNextFenceValue() - 1);

	GD3D12SwapChain.Present();
}

// -----------------------------------------------------------------------------
// EndFrame: advance frame indices and perform end-of-frame housekeeping
// -----------------------------------------------------------------------------
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
	GD3D12ConstantBufferManager.Shutdown();
	GD3D12FrameResourceManager.Shutdown();
	GD3D12SwapChain.Shutdown();
	GWindow.Shutdown();
	GD3D12DescriptorHeapManager.Shutdown();
	GD3D12Rhi.Shutdown();
}