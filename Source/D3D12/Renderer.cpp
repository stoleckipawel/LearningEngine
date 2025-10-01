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
	CreateDescriptorHeaps();
	//CreateConstantBuffers(cbvHeap);
	CreateFrameBuffers();

	//W8 single frame for resources to be ready for rendering
	WaitForPreviousFrame();
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
	//ReleaseConstantBuffers();
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
	
	
	GRHI.CmdList->SetGraphicsRootSignature(rootSignature.rootSignature);
	SetViewport(GRHI.CmdList);

	SetBackBufferRTV(GRHI.CmdList);	

	ClearBackBuffer(GRHI.CmdList);

	vertecies.Set(GRHI.CmdList);

	GRHI.CmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	GRHI.CmdList->DrawIndexedInstanced(6, 1, 0, 4, 0); // draw second quad
	
	/*	
	pso.Set(GRHI.CmdList);
	ID3D12DescriptorHeap* heaps[] = { cbvHeap.heap.Get() };
	GRHI.CmdList->SetDescriptorHeaps(_countof(heaps), heaps);
	GRHI.CmdList->SetGraphicsRootDescriptorTable(0, cbvHeap.heap->GetGPUDescriptorHandleForHeapStart());
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
	GRHI.Flush();
	ReleaseFrameBuffers();
	CreateFrameBuffers();
}

void FRenderer::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.	
	const UINT64 fenceValueTmp = GRHI.FenceValue;
	ThrowIfFailed(GRHI.CmdQueue->Signal(GRHI.Fence, fenceValueTmp), "RHI: Failed To Signal Command Queue");
	GRHI.FenceValue++;
    // Wait until the previous frame is finished.
    if (GRHI.Fence->GetCompletedValue() < fenceValueTmp)
    {
        ThrowIfFailed(GRHI.Fence->SetEventOnCompletion(fenceValueTmp, GRHI.FenceEvent), "RHI: Failed To Signal Command Queue");
        WaitForSingleObject(GRHI.FenceEvent, INFINITE);
    }
	GSwapChain.UpdateBackBufferIndex();
}

// Render the scene.
void FRenderer::OnRender()
{
	OnUpdate();

	// Prepare the command list to render a new frame.
    ThrowIfFailed(GRHI.CmdAllocator->Reset(), "Renderer: Failed To Reset Command Allocator");
    ThrowIfFailed(GRHI.CmdList->Reset(GRHI.CmdAllocator.Get(), pso.pso.Get()), "Renderer: Failed To Reset Command List");

	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Finalize the command list.
	ThrowIfFailed(GRHI.CmdList->Close(), "Failed To Close Command List");

	// Execute the command list.
	GRHI.ExecuteCommandList();

	// Present the frame.
	GSwapChain.Present();

	WaitForPreviousFrame();
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