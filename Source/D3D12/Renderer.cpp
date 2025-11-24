#include "Renderer.h"
#include "DebugLayer.h"
#include "RHI.h"
#include "Window.h"
#include "ConstantBufferManager.h"

Renderer GRenderer;

void Renderer::LoadGeometry()
{
	m_vertecies.Upload();
}

void Renderer::LoadTextures()
{
	m_texture.Initialize("Assets/Textures/Test1.png", 0);
}

void Renderer::LoadSamplers()
{
	m_sampler = Sampler(0);
}

void Renderer::LoadShaders()
{
	m_vertexShader = ShaderCompiler(L"Shaders/VertShader.hlsl", "vs_5_0", "main");//To Do check 6.0 shaders
	m_pixelShader = ShaderCompiler(L"Shaders/PixShader.hlsl", "ps_5_0", "main");
}

void Renderer::CreateRootSignatures()
{
	m_rootSignature = RootSignature();
	m_rootSignature.Create();
}
void Renderer::ReleaseRootSignatures()
{
	m_rootSignature.Get().Release();
}

void Renderer::CreatePSOs()
{
	m_pso.Create(m_vertecies, m_rootSignature.Get(), m_vertexShader, m_pixelShader);
}

void Renderer::ReleasePSOs()
{
	m_pso.Get().Release();
}

void Renderer::PostLoad()
{
	GRHI.CloseCommandLists();
	GRHI.ExecuteCommandList();	
	GRHI.Flush();
}

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

void Renderer::Release()
{
	ReleaseRootSignatures();
	ReleasePSOs();
	GDescriptorHeapManager.Release();
	GConstantBufferManager.Release();
}

void Renderer::SetViewport()
{
	//Rasterizer State: Viewport
	D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
	GRHI.GetCommandList()->RSSetViewports(1, &viewport);

	//Rasterizer State: Scissor  
	D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
	GRHI.GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void Renderer::SetBackBufferRTV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_depthStencil.GetCPUHandle();
	GRHI.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &depthStencilHandle);
}

void Renderer::BindDescriptorTables()
{
	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		0, 
		m_texture.GetGPUHandle());

	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		1,
		m_sampler.GetGPUHandle());

	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		2,
		GConstantBufferManager.VertexConstantBuffers[GSwapChain.GetBackBufferIndex()].GetGPUHandle());

	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		3,
		GConstantBufferManager.PixelConstantBuffers[GSwapChain.GetBackBufferIndex()].GetGPUHandle());
}

void Renderer::PopulateCommandList()
{
	GSwapChain.SetRenderTargetState();
	
	GRHI.GetCommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	SetViewport();

	SetBackBufferRTV();

	GSwapChain.Clear();
	m_depthStencil.Clear();

	m_vertecies.Set();

	GDescriptorHeapManager.SetShaderVisibleHeaps();
	BindDescriptorTables();

	m_pso.Set();

	GRHI.GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	GSwapChain.SetPresentState();
}

void Renderer::CreateFrameBuffers()
{
	m_depthStencil.Initialize(0);
}

void Renderer::OnUpdate()
{
	m_frameIndex++;
	GConstantBufferManager.Update(m_frameIndex);
}

void Renderer::OnResize()
{
	GRHI.Flush();
	CreateFrameBuffers();
}

// Render the scene.
void Renderer::OnRender()
{
	OnUpdate();

	GRHI.WaitForGPU();

	// Prepare the command list to render a new frame.
    ThrowIfFailed(GRHI.GetCommandAllocator()->Reset(), "Renderer: Failed To Reset Command Allocator");
    ThrowIfFailed(GRHI.GetCommandList()->Reset(GRHI.GetCommandAllocator().Get(), m_pso.Get().Get()), "Renderer: Failed To Reset Command List");

	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Finalize the command list.
	ThrowIfFailed(GRHI.GetCommandList()->Close(), "Failed To Close Command List");

	// Execute the command list.
	GRHI.ExecuteCommandList();

	// Update Fence Value when GPU completes work
	GRHI.Signal();

	// Present the frame.
	GSwapChain.Present();

	// Update the frame index.
	GSwapChain.UpdateCurrentBackBufferIndex();
}

void Renderer::Shutdown()
{
	GRHI.Flush();

	Renderer::Release();
	GSwapChain.Shutdown();
	GWindow.Shutdown();
	GRHI.Shutdown();
	GDebugLayer.Shutdown();
}