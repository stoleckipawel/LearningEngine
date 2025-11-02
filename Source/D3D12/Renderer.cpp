#include "Renderer.h"
#include "DebugLayer.h"
#include "RHI.h"
#include "Window.h"
#include "ConstantBufferManager.h"

Renderer GRenderer;

void Renderer::LoadGeometry()
{
	vertecies.Upload();
}

void Renderer::LoadTextures()
{
	texture.Initialize("Assets/Textures/ColorCheckerBoard.png", GRHI.GetCommandList(), 0);
}

void Renderer::LoadSamplers()
{
	sampler = Sampler(0);
}

void Renderer::LoadShaders()
{
	vertexShader = ShaderCompiler(L"Shaders/VertShader.hlsl", "vs_5_0", "main");//To Do check 6.0 shaders
	pixelShader = ShaderCompiler(L"Shaders/PixShader.hlsl", "ps_5_0", "main");
}

void Renderer::CreateRootSignatures()
{
	rootSignature = RootSignature();
	rootSignature.Create();
}
void Renderer::ReleaseRootSignatures()
{
	rootSignature.rootSignature.Release();
}

void Renderer::CreatePSOs()
{
	pso.Create(vertecies, rootSignature.rootSignature, vertexShader, pixelShader);
}

void Renderer::ReleasePSOs()
{
	pso.pso.Release();
}

void Renderer::CreateCommandLists()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i) 
	{ 
		ThrowIfFailed(GRHI.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, GRHI.CmdAllocator[i].Get(), pso.pso.Get(), IID_PPV_ARGS(&GRHI.CmdList[i])), "RHI: Failed To Create Command List");
	}
}

void Renderer::CloseCommandLists()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i) 
	{ 
		ThrowIfFailed(GRHI.CmdList[i]->Close(), "RHI: Failed To Close Command List");
	}
}

void Renderer::ReleaseCommandLists()
{
	for (size_t i = 0; i < NumFramesInFlight; ++i) 
	{ 
		GRHI.CmdAllocator[i].Release();
	}
}

void Renderer::CreateDepthStencilBuffer()
{
	{
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
	}

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	{
		depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;
	}

	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	{
		heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDefaultProperties.CreationNodeMask = 0;
		heapDefaultProperties.VisibleNodeMask = 0;
	}

	{
		depthStencilResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilResourceDesc.MipLevels = 1;
		depthStencilResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilResourceDesc.Height = GWindow.GetHeight();
		depthStencilResourceDesc.Width = GWindow.GetWidth();
		depthStencilResourceDesc.DepthOrArraySize = 1;
		depthStencilResourceDesc.SampleDesc.Count = 1;
		depthStencilResourceDesc.SampleDesc.Quality = 0;
		depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;		
	}


	GRHI.Device->CreateCommittedResource(
		&heapDefaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&depthStencilBuffer)
	);

	if(FAILED(depthStencilBuffer))
	{
		std::string message = "Create Depth Stencil Buffer: Failed to Create Resource";
		LogError(message, ELogType::Warning);
	}

	DepthStencilHandle = GDescriptorHeapManager.GetDepthStencilViewHeap().heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.Device->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, DepthStencilHandle);
}

void Renderer::PostLoad()
{
	CloseCommandLists();
	GRHI.ExecuteCommandList();	
	GRHI.Flush();
}

void Renderer::Load()
{
	CreateRootSignatures();
	LoadGeometry();
	LoadShaders();
	CreateCommandLists();
	GDescriptorHeapManager.Initialize();
	GConstantBufferManager.Initialize();	
	LoadTextures();
	LoadSamplers();
	CreatePSOs();
	CreateFrameBuffers();
	PostLoad();
}

void Renderer::Release()
{
	ReleaseCommandLists();
	ReleaseRootSignatures();
	ReleasePSOs();
	ReleaseFrameBuffers();
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

void Renderer::ClearBackBuffer()
{
	//Clear backbuffer 
	float clearColor[4] = {255.0f, 0.5f, 0.5f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	GRHI.GetCommandList()->ClearRenderTargetView(backBufferRTVHandle, clearColor, 0, nullptr);
}

void Renderer::ClearDepthStencilBuffer()
{
	DepthStencilHandle = GDescriptorHeapManager.GetDepthStencilViewHeap().heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.GetCommandList()->ClearDepthStencilView(DepthStencilHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void Renderer::SetBackBufferRTV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	DepthStencilHandle = GDescriptorHeapManager.GetDepthStencilViewHeap().heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.GetCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &DepthStencilHandle);
}

void Renderer::BindDescriptorTables()
{
	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		0, 
		GConstantBufferManager.VertexConstantBuffers[GSwapChain.GetBackBufferIndex()].GetGPUHandle());

	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		1, 
		GConstantBufferManager.PixelConstantBuffers[GSwapChain.GetBackBufferIndex()].GetGPUHandle());

	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		2, 
		sampler.GetGPUHandle());

	GRHI.GetCommandList()->SetGraphicsRootDescriptorTable(
		3, 
		texture.GetGPUHandle());
}

void Renderer::PopulateCommandList()
{
	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	
	GRHI.GetCommandList()->SetGraphicsRootSignature(rootSignature.rootSignature);

	SetViewport();

	SetBackBufferRTV();

	ClearBackBuffer();
	ClearDepthStencilBuffer();

	vertecies.Set();

	GDescriptorHeapManager.SetShaderVisibleHeaps();
	BindDescriptorTables();

	pso.Set();

	GRHI.GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
}

void Renderer::CreateFrameBuffers()
{
	CreateDepthStencilBuffer();
}

void Renderer::ReleaseFrameBuffers()
{
	depthStencilBuffer.Release();
}

void Renderer::OnUpdate()
{
	BackBufferFrameIndex++;
	GConstantBufferManager.Update(BackBufferFrameIndex);
}

void Renderer::OnResize()
{
	GRHI.Flush();
	ReleaseFrameBuffers();
	CreateFrameBuffers();
}

// Render the scene.
void Renderer::OnRender()
{
	OnUpdate();

	GRHI.WaitForGPU();

	// Prepare the command list to render a new frame.
    ThrowIfFailed(GRHI.GetCommandAllocator()->Reset(), "Renderer: Failed To Reset Command Allocator");
    ThrowIfFailed(GRHI.GetCommandList()->Reset(GRHI.GetCommandAllocator().Get(), pso.pso.Get()), "Renderer: Failed To Reset Command List");

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