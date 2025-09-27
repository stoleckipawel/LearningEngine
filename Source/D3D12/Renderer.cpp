#include "Renderer.h"
#include "DebugLayer.h"
#include "RHI.h"
#include "Window.h"
#include "SwapChain.h"

FRenderer GRenderer;

bool FRenderer::Initialize()
{
	//Initialize Window
	ThrowIfFailed(GWindow.Initialize(), "Failed To Initialize Window");

	// Initialize RHI
	ThrowIfFailed(GRHI.Initialize(), "Failed To Initialize RHI");

	bInitialized = true;
	return true;
}

void FRenderer::LoadGeometry()
{
	vertecies.Upload();
}

void FRenderer::UnloadGeometry()
{
	vertecies.Release();
}

void FRenderer::LoadTextures()
{
	texture.Load("Assets/Textures/ColorCheckerBoard.png");
}

void FRenderer::UnloadTextures()
{
	texture.Release();
}

void FRenderer::LoadShaders()
{
	vertexShader = FShaderCompiler(L"Shaders/VertShader.hlsl", "vs_5_0", "main");
	pixelShader = FShaderCompiler(L"Shaders/PixShader.hlsl", "ps_5_0", "main");
}

void FRenderer::LoadAssets()
{
	LoadGeometry();
	LoadTextures();
	LoadShaders();
}

void FRenderer::CreateRootSignatures()
{
	rootSignature = FRootSignature();
	rootSignature.Create();
}
void FRenderer::ReleaseRootSignatures()
{
	rootSignature.rootSignature.Release();
}

void FRenderer::CreateDescriptorHeaps()
{
	cbvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"CBVHeap");
	samplerHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"SamplerHeap");
	dsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"DepthStencilHeap");
}

void FRenderer::ReleaseDescriptorHeaps()
{
	cbvHeap.heap.Release();
	samplerHeap.heap.Release();
	dsvHeap.heap.Release();
}

void FRenderer::CreatePSOs()
{
	pso.Create(vertecies, rootSignature.rootSignature, vertexShader, pixelShader);
}

void FRenderer::ReleasePSOs()
{
	pso.pso.Release();
}

void FRenderer::CreateDepthStencilBuffer()
{
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

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
	dsvHandle = dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.Device->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, dsvHandle);
}

void FRenderer::CreateConstantBuffers(FDescriptorHeap& descriptorHeap)
{
	ConstantBuffer.Create();
	ConstantBuffer.CreateConstantBufferView(cbvHeap);
}

void FRenderer::ReleaseConstantBuffers()
{
	ConstantBuffer.Resource.Release();
}

void FRenderer::Load()
{
	LoadAssets();

	CreateRootSignatures();
	CreatePSOs();
	CreateDescriptorHeaps();
	CreateConstantBuffers(cbvHeap);
}

void FRenderer::Unload()
{
	//Unloads previously loaded assets from disc
	UnloadGeometry();
	UnloadTextures();
}

void FRenderer::Release()
{
	GRHI.Flush();
	ReleaseRootSignatures();
	ReleasePSOs();
	ReleaseFrameBuffers();
	ReleaseDescriptorHeaps();
	ReleaseConstantBuffers();
}

void FRenderer::SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Rasterizer State: Viewport
	D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
	cmdList->RSSetViewports(1, &viewport);
	//Rasterizer State: Scissor  
	D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
	cmdList->RSSetScissorRects(1, &scissorRect);
}

void FRenderer::ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Clear backbuffer 
	float clearColor[4] = {255.0f, 0.5f, 0.5f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	cmdList->ClearRenderTargetView(backBufferRTVHandle, clearColor, 0, nullptr);

	dsvHandle = dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void FRenderer::SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	cmdList->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &dsvHandle);
}
void FRenderer::OnFrameBegin()
{
	//Sets BackBuffer to Render Target State
	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void FRenderer::OnFrameEnd()
{
	//Sets BackbBuffer to Present State
	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
}
void FRenderer::PopulateCommandList()
{
	OnFrameBegin();
	/*
	ClearBackBuffer(GRHI.CmdList);
	
	vertecies.Set(GRHI.CmdList);

	pso.Set(GRHI.CmdList);

	GRHI.CmdList->SetGraphicsRootSignature(rootSignature.rootSignature);
	ID3D12DescriptorHeap* heaps[] = { cbvHeap.heap.Get() };
	GRHI.CmdList->SetDescriptorHeaps(_countof(heaps), heaps);
	GRHI.CmdList->SetGraphicsRootDescriptorTable(0, cbvHeap.heap->GetGPUDescriptorHandleForHeapStart());
	
	SetViewport(GRHI.CmdList);
	SetBackBufferRTV(GRHI.CmdList);

	//GRHI.CmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	//GRHI.CmdList->DrawIndexedInstanced(6, 1, 0, 4, 0); // draw second quad

	
	*/
	OnFrameEnd();
}

void FRenderer::CreateFrameBuffers()
{
	CreateDepthStencilBuffer();
}

void FRenderer::ReleaseFrameBuffers()
{
	depthStencilBuffer.Release();
}

void FRenderer::UpdateRainbowColor()
{
	float speed = FrameIndex * 0.02f; // Speed of the color change

	FConstantBufferData data;
	data.color.x = 0.5f + 0.5f * sinf(speed);
	data.color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	data.color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	data.color.w = 1.0f;
	ConstantBuffer.Update(data);
}

void FRenderer::OnUpdate()
{
	FrameIndex++;
	UpdateRainbowColor();
}

void FRenderer::OnResize()
{
	GRHI.Flush();
	ReleaseFrameBuffers();
	CreateFrameBuffers();
}

// Render the scene.
void FRenderer::OnRender()
{
	OnUpdate();

	//#ToDo: scene.Update(); 
	// Camera Update 
	// Models Update, 
	// Particle Update
	// Load/Release Resources (Textures Models)

	GRHI.InitializeCommandList();//Reset Command List & Command Allocator

	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	GRHI.CmdList->Close();

	// Execute the command list.
	//FRHI::Get().ExecuteCommandList();

	// Present the frame.
	GSwapChain.Present();
}

void FRenderer::Shutdown()
{
	FRenderer::Release();
	FRenderer::Unload();
	GSwapChain.Shutdown();
	GWindow.Shutdown();
	GRHI.Shutdown();
	GDebugLayer.Shutdown();
}