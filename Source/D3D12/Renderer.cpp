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

	dsvHandle = dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	GRHI.Device->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, dsvHandle);
}

void FRenderer::CreateConstantBuffers(FDescriptorHeap& descriptorHeap)
{
	//ConstantBuffer.Create();
	//ConstantBuffer.CreateConstantBufferView(cbvHeap);
}

void FRenderer::ReleaseConstantBuffers()
{
	//ConstantBuffer.Resource.Release();
}

void FRenderer::Load()
{
	LoadAssets();

	CreateRootSignatures();
	CreatePSOs();
	CreateCommandLists();
	CreateDescriptorHeaps();
	//CreateConstantBuffers(cbvHeap);
	CreateFrameBuffers();

	//W8 single frame for resources to be ready for rendering
	WaitForGPU();
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
	//ReleaseConstantBuffers();
}

void FRenderer::SetViewport(ID3D12GraphicsCommandList7* cmdList)
{
	//Rasterizer State: Viewport
	D3D12_VIEWPORT viewport = GSwapChain.GetDefaultViewport();
	cmdList->RSSetViewports(1, &viewport);
	//Rasterizer State: Scissor  
	D3D12_RECT scissorRect = GSwapChain.GetDefaultScissorRect();
	cmdList->RSSetScissorRects(1, &scissorRect);
}

void FRenderer::ClearBackBuffer(ID3D12GraphicsCommandList7* cmdList)
{
	//Clear backbuffer 
	float clearColor[4] = {255.0f, 0.5f, 0.5f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	cmdList->ClearRenderTargetView(backBufferRTVHandle, clearColor, 0, nullptr);

	dsvHandle = dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void FRenderer::SetBackBufferRTV(ID3D12GraphicsCommandList7* cmdList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = GSwapChain.GetBackbufferRTVHandle();
	dsvHandle = dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &dsvHandle);
}

void FRenderer::PopulateCommandList()
{
	//Sets BackbBuffer to Present State
	GRHI.SetBarrier(
		GSwapChain.GetBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	
	GRHI.GetCurrentCommandList()->SetGraphicsRootSignature(rootSignature.rootSignature);
	SetViewport(GRHI.GetCurrentCommandList());

	SetBackBufferRTV(GRHI.GetCurrentCommandList());

	ClearBackBuffer(GRHI.GetCurrentCommandList());

	vertecies.Set(GRHI.GetCurrentCommandList());

	GRHI.GetCurrentCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	/*	
	pso.Set(GRHI.GetCurrentCommandList());
	ID3D12DescriptorHeap* heaps[] = { cbvHeap.heap.Get() };
	GRHI.GetCurrentCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
	GRHI.GetCurrentCommandList()->SetGraphicsRootDescriptorTable(0, cbvHeap.heap->GetGPUDescriptorHandleForHeapStart());
	*/

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

	FConstantBufferData data;
	data.color.x = 0.5f + 0.5f * sinf(speed);
	data.color.y = 0.5f + 0.5f * sinf(speed + 2.0f);
	data.color.z = 0.5f + 0.5f * sinf(speed + 4.0f);
	data.color.w = 1.0f;
	//ConstantBuffer.Update(data);
}

// Update frame-based values.
void FRenderer::OnUpdate()
{
	FrameIndex++;
	UpdateRainbowColor();
}

void FRenderer::OnResize()
{
	//Wait for GPU to finish with all resources?
	ReleaseFrameBuffers();
	CreateFrameBuffers();
}

// Wait for pending GPU work to complete.
void FRenderer::WaitForGPU()
{	
	// Schedule a Signal command in the queue.
	ThrowIfFailed(GRHI.CmdQueue->Signal(GRHI.Fence, GRHI.GetCurrentFenceValue()), "RHI: Failed To Signal Command Queue");

    // Wait until the fence has been processed.
    ThrowIfFailed(GRHI.Fence->SetEventOnCompletion(GRHI.GetCurrentFenceValue(), GRHI.FenceEvent), "RHI: Failed To Signal Command Queue");
    WaitForSingleObjectEx(GRHI.FenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	GRHI.FenceValues[GSwapChain.GetCurrentBackBufferIndex()]++;
}

// Prepare to render the next frame.
void FRenderer::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = GRHI.GetCurrentFenceValue();
	ThrowIfFailed(GRHI.CmdQueue->Signal(GRHI.Fence, currentFenceValue), "RHI: Failed To Signal Command Queue");

	// Update the frame index.
	GSwapChain.UpdateCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (GRHI.Fence->GetCompletedValue() < GRHI.GetCurrentFenceValue())
	{
		ThrowIfFailed(GRHI.Fence->SetEventOnCompletion(GRHI.GetCurrentFenceValue(), GRHI.FenceEvent), "RHI: Failed To Signal Command Queue");
		WaitForSingleObjectEx(GRHI.FenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	GRHI.FenceValues[GSwapChain.GetCurrentBackBufferIndex()] = currentFenceValue + 1;
}

// Render the scene.
void FRenderer::OnRender()
{
	OnUpdate();

	// Prepare the command list to render a new frame.
    ThrowIfFailed(GRHI.GetCurrentCommandAllocator()->Reset(), "Renderer: Failed To Reset Command Allocator");
    ThrowIfFailed(GRHI.GetCurrentCommandList()->Reset(GRHI.GetCurrentCommandAllocator().Get(), pso.pso.Get()), "Renderer: Failed To Reset Command List");

	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Finalize the command list.
	ThrowIfFailed(GRHI.CmdList[GSwapChain.GetCurrentBackBufferIndex()]->Close(), "Failed To Close Command List");

	// Execute the command list.
	GRHI.ExecuteCommandList();

	// Present the frame.
	GSwapChain.Present();

	// Prepare to render the next frame.
	MoveToNextFrame();
}

void FRenderer::Shutdown()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForGPU();

	// Release all resources.
	FRenderer::Release();
	FRenderer::Unload();
	GSwapChain.Shutdown();
	GWindow.Shutdown();
	GRHI.Shutdown();
	GDebugLayer.Shutdown();
}