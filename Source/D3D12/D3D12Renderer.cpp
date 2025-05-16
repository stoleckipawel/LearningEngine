#include "D3D12Renderer.h"
#include "D3D12DebugLayer.h"
#include "D3D12Context.h"
#include "D3D12Window.h"


bool D3D12Renderer::Initialize()
{

	if (FAILED(D3D12DebugLayer::Get().Initialize()))
	{
		return false;
	}

	if(FAILED(D3D12Context::Get().Initialize()))
	{
		return false;
	}
		
	if(FAILED(D3D12Window::Get().Initialize()))
	{
		return false;
	}
	D3D12Window::Get().SetFullScreen(false);

	return true;
}

void D3D12Renderer::SetDescriptorHeaps(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	ID3D12DescriptorHeap* heaps[] = { srvHeap.heap.Get(), samplerHeap.heap.Get(), dsvHeap.heap.Get()};
	//cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

	//cmdList->SetGraphicsRootDescriptorTable(1, srvHeap.heap->GetGPUDescriptorHandleForHeapStart());
	//cmdList->SetGraphicsRootDescriptorTable(2, samplerHeap->GetGPUDescriptorHandleForHeapStart());
}

void D3D12Renderer::LoadGeometry()
{
	vertecies.Upload();
}

void D3D12Renderer::LoadTextures()
{
	texture.Load("Assets/Textures/ColorCheckerBoard.png");
}

void D3D12Renderer::LoadShaders()
{
	vertexShader = D3D12ShaderCompiler(L"Shaders/VertShader.hlsl", "vs_5_0", "main");
	pixelShader = D3D12ShaderCompiler(L"Shaders/PixShader.hlsl", "ps_5_0", "main");
}

void D3D12Renderer::LoadRootSignature()
{
	rootSignature = D3D12RootSignature();
	rootSignature.Create();
}

void D3D12Renderer::CreateDescriptorHeaps()
{
	srvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32, L"SRVHeap");
	samplerHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32, L"SamplerHeap");
	dsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, L"DepthStencilHeap");
}

void D3D12Renderer::CreatePSO()
{
	pso.Create(vertecies, rootSignature.rootSignature, vertexShader, pixelShader);
}

void D3D12Renderer::CreateDepthStencilBuffer()
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
	depthStencilResourceDesc.Height = D3D12Window::Get().GetHeight();
	depthStencilResourceDesc.Width = D3D12Window::Get().GetWidth();
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.SampleDesc.Count = 1;
	depthStencilResourceDesc.SampleDesc.Quality = 0;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	D3D12Context::Get().GetDevice()->CreateCommittedResource(
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
		LogError(message);
	}
	dsvHandle = dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	D3D12Context::Get().GetDevice()->CreateDepthStencilView(depthStencilBuffer, &depthStencilDesc, dsvHandle);
}

void D3D12Renderer::Load()
{
	LoadGeometry();
	LoadTextures();
	LoadShaders();
	LoadRootSignature();
	CreatePSO();
	CreateDescriptorHeaps();
}

void D3D12Renderer::SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Rasterizer State: Viewport
	D3D12_VIEWPORT viewport = D3D12Window::Get().GetDefaultViewport();
	cmdList->RSSetViewports(1, &viewport);
	//Rasterizer State: Scissor  
	D3D12_RECT scissorRect = D3D12Window::Get().GetDefaultScissorRect();
	cmdList->RSSetScissorRects(1, &scissorRect);
}

void D3D12Renderer::ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Clear backbuffer 
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = D3D12Window::Get().GetBackbufferRTVHandle();
	cmdList->ClearRenderTargetView(backBufferRTVHandle, clearColor, 0, nullptr);

	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void D3D12Renderer::SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = D3D12Window::Get().GetBackbufferRTVHandle();
	cmdList->OMSetRenderTargets(1, &backBufferRTVHandle, FALSE, &dsvHandle);
}

void D3D12Renderer::SetShaderParams(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	cmdList->SetGraphicsRootSignature(rootSignature.rootSignature);

	//float color[] = { 1.0f, 1.0f, 0.0f };
	//cmdList->SetGraphicsRoot32BitConstants(0, 3, color, 0);
	SetDescriptorHeaps(cmdList);
}

void D3D12Renderer::Draw(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	ClearBackBuffer(cmdList);

	vertecies.Set(cmdList);

	pso.Set(cmdList);

	SetShaderParams(cmdList);

	SetViewport(cmdList);
	SetBackBufferRTV(cmdList);

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	cmdList->DrawIndexedInstanced(6, 1, 0, 4, 0); // draw second quad
}

void D3D12Renderer::CreateFrameBuffers()
{
	CreateDepthStencilBuffer();
}

void D3D12Renderer::Render()
{
	//On Frame Begin
	CreateFrameBuffers();
	auto cmdList = D3D12Context::Get().InitializeCommandList();
	D3D12Window::Get().SetBackBufferStateToRT(cmdList);

	//#ToDo: scene.Update();: 
	// Camera Update 
	// Models Update, 
	// Particle Update
	
	//ToDo: Load/Release Resources (Textures Models)

	Draw(cmdList);

	//On Frame End
	D3D12Window::Get().SetBackBufferStateToPresent(cmdList);
	D3D12Context::Get().ExecuteCommandList();
	D3D12Window::Get().Present();
}

void D3D12Renderer::Shutdown()
{
	D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());

	//vertexBuffer->Release();
	//uploadBuffer->Release();
	//pso->Release();
	//D3D12Window::Get().Shutdown();
	//D3D12Context::Get().Shutdown();
	//D3D12DebugLayer::Get().Shutdown();
}