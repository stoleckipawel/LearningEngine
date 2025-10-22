#include "Renderer.h"
#include "DebugLayer.h"
#include "RHI.h"
#include "Window.h"
#include "SwapChain.h"

FRenderer GRenderer;

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
	vertexShader = FShaderCompiler(L"Shaders/VertShader.hlsl", "vs_5_0", "main");//To Do check 6.0 shaders
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
	ConstantBufferHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, BufferingCount * NumConstantBuffers, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"ConstantBufferHeap");
	SamplerHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"SamplerHeap");
	DepthStencilViewHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"DepthStencilHeap");
}

void FRenderer::ReleaseDescriptorHeaps()
{
	ConstantBufferHeap.heap.Release();
	SamplerHeap.heap.Release();
	DepthStencilViewHeap.heap.Release();
}

void FRenderer::CreatePSOs()
{
	pso.Create(vertecies, rootSignature.rootSignature, vertexShader, pixelShader);
}

void FRenderer::ReleasePSOs()
{
	pso.pso.Release();
}

void FRenderer::CreateCommandLists()
{
	for (size_t i = 0; i < BufferingCount; ++i) 
	{ 
		// Create the command list.
		ThrowIfFailed(GRHI.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, GRHI.CmdAllocator[i].Get(), pso.pso.Get(), IID_PPV_ARGS(&GRHI.CmdList[i])), "RHI: Failed To Create Command List");
		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		ThrowIfFailed(GRHI.CmdList[i]->Close(), "RHI: Failed To Close Command List");
	}
}

void FRenderer::ReleaseCommandLists()
{
	for (size_t i = 0; i < BufferingCount; ++i) 
	{ 
		GRHI.CmdAllocator[i].Release();
	}
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

	DepthStencilHandle = DepthStencilViewHeap.heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.Device->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, DepthStencilHandle);
}

void FRenderer::CreateConstantBuffers()
{
	for (size_t i = 0; i < BufferingCount; ++i)
	{
		//Create Vertex Constant Buffer
		FConstantBuffer<FVertexConstantBuffer> vertexConstantBuffer;
		vertexConstantBuffer.Initialize(0);
		vertexConstantBuffer.CreateConstantBufferView(ConstantBufferHeap.GetCPUHandle(i, vertexConstantBuffer.HandleIndex));
		VertexConstantBuffers[i] = vertexConstantBuffer;

		//Create Pixel Constant Buffer
		FConstantBuffer<FPixelConstantBuffer> pixelConstantBuffer;
		pixelConstantBuffer.Initialize(1);
		pixelConstantBuffer.CreateConstantBufferView(ConstantBufferHeap.GetCPUHandle(i, pixelConstantBuffer.HandleIndex));
		PixelConstantBuffers[i] = pixelConstantBuffer;
	}
}

void FRenderer::ReleaseConstantBuffers()
{
	for (size_t i = 0; i < BufferingCount; ++i)
	{
		VertexConstantBuffers[i].Resource.Release();
		PixelConstantBuffers[i].Resource.Release();
	}
}

void FRenderer::Load()
{
	LoadAssets();

	CreateRootSignatures();
	CreatePSOs();
	CreateCommandLists();
	CreateDescriptorHeaps();
	CreateConstantBuffers();
	CreateFrameBuffers();

	GRHI.Flush();
}

void FRenderer::Unload()
{
	//Unloads previously loaded assets from disc
	UnloadGeometry();
	UnloadTextures();
}

void FRenderer::Release()
{
	ReleaseCommandLists();
	ReleaseRootSignatures();
	ReleasePSOs();
	ReleaseFrameBuffers();
	ReleaseDescriptorHeaps();
	ReleaseConstantBuffers();
}

void FRenderer::SetViewport()
{
	//Rasterizer State: Viewport
	D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
	GRHI.GetCurrentCommandList()->RSSetViewports(1, &viewport);

	//Rasterizer State: Scissor  
	D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
	GRHI.GetCurrentCommandList()->RSSetScissorRects(1, &scissorRect);
}

void FRenderer::ClearBackBuffer()
{
	//Clear backbuffer 
	float clearColor[4] = {255.0f, 0.5f, 0.5f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	GRHI.GetCurrentCommandList()->ClearRenderTargetView(backBufferRTVHandle, clearColor, 0, nullptr);
}

void FRenderer::ClearDepthStencilBuffer()
{
	DepthStencilHandle = DepthStencilViewHeap.heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.GetCurrentCommandList()->ClearDepthStencilView(DepthStencilHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void FRenderer::SetBackBufferRTV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	DepthStencilHandle = DepthStencilViewHeap.heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.GetCurrentCommandList()->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &DepthStencilHandle);
}

void FRenderer::SetDescriptorHeaps()
{
	ID3D12DescriptorHeap* heaps[] = { ConstantBufferHeap.heap.Get() };
	GRHI.GetCurrentCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
}

void FRenderer::BindDescriptorTables()
{
	ConstantBufferHeap.GetCurrentFrameGPUHandle(0);
	GRHI.GetCurrentCommandList()->SetGraphicsRootDescriptorTable(0, ConstantBufferHeap.GetCurrentFrameGPUHandle(VertexConstantBuffers[GSwapChain.GetCurrentBackBufferIndex()].HandleIndex));
	GRHI.GetCurrentCommandList()->SetGraphicsRootDescriptorTable(1, ConstantBufferHeap.GetCurrentFrameGPUHandle(PixelConstantBuffers[GSwapChain.GetCurrentBackBufferIndex()].HandleIndex));
}

void FRenderer::PopulateCommandList()
{
	//Sets BackbBuffer to Present State
	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	
	GRHI.GetCurrentCommandList()->SetGraphicsRootSignature(rootSignature.rootSignature);

	SetViewport();

	SetBackBufferRTV();

	ClearBackBuffer();
	ClearDepthStencilBuffer();

	vertecies.Set();

	SetDescriptorHeaps();
	BindDescriptorTables();

	pso.Set();

	GRHI.GetCurrentCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	//Sets BackbBuffer to Present State
	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
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

	//Update Vertex Constant Buffers
	FVertexConstantBuffer vertexData;
	vertexData.WorldMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	vertexData.ViewMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	vertexData.ProjectionMTX = XMFLOAT4X4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.f, 0.0f);
	VertexConstantBuffers[GSwapChain.GetCurrentBackBufferIndex()].Update(vertexData);

	//Update Pixel Constant Buffers
	FPixelConstantBuffer pixelData;
	pixelData.Color.x = 0.5f + 0.5f * sinf(speed);
	pixelData.Color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	pixelData.Color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	pixelData.Color.w = 1.0f;
	PixelConstantBuffers[GSwapChain.GetCurrentBackBufferIndex()].Update(pixelData);
}

// Update frame-based values.
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

	GRHI.WaitForGPU();

	// Prepare the command list to render a new frame.
    ThrowIfFailed(GRHI.GetCurrentCommandAllocator()->Reset(), "Renderer: Failed To Reset Command Allocator");
    ThrowIfFailed(GRHI.GetCurrentCommandList()->Reset(GRHI.GetCurrentCommandAllocator().Get(), pso.pso.Get()), "Renderer: Failed To Reset Command List");

	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Finalize the command list.
	ThrowIfFailed(GRHI.CmdList[GSwapChain.GetCurrentBackBufferIndex()]->Close(), "Failed To Close Command List");

	// Execute the command list.
	GRHI.ExecuteCommandList();

	// Update Fence Value when GPU completes work
	GRHI.Signal();

	// Present the frame.
	GSwapChain.Present();

	// Update the frame index.
	GSwapChain.UpdateCurrentBackBufferIndex();
}

void FRenderer::Shutdown()
{
	GRHI.Flush();

	// Release all resources.
	FRenderer::Release();
	FRenderer::Unload();
	GSwapChain.Shutdown();
	GWindow.Shutdown();
	GRHI.Shutdown();
	GDebugLayer.Shutdown();
}