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
	ID3D12DescriptorHeap* heaps[] = { srvHeap.heap.Get(), samplerHeap.heap.Get() };
	cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

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
	srvHeap = D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32);
	samplerHeap = D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32);
}

void D3D12Renderer::CreatePSO()
{
	pso.Create(vertecies, rootSignature.rootSignature, vertexShader, pixelShader);
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
}

void D3D12Renderer::SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTVHandle = D3D12Window::Get().GetBackbufferRTVHandle();
	cmdList->OMSetRenderTargets(1, &backBufferRTVHandle, true, nullptr);
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
}

void D3D12Renderer::Render()
{
	//On Frame Begin
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