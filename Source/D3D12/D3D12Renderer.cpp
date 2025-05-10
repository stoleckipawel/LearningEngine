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

void D3D12Renderer::CreateRootSignature()
{
	D3D12Shader rootSignatureShader = D3D12Shader("RootSignature.cso");

	D3D12Context::Get().GetDevice()->CreateRootSignature(
		0,
		rootSignatureShader.GetBuffer(),
		rootSignatureShader.GetSize(),
		IID_PPV_ARGS(&rootSignature));
}

void D3D12Renderer::Load()
{
	vertecies.Upload();	
	texture.Load("Assets/Textures/ColorCheckerBoard.png");

	D3D12Shader vertexShader = D3D12Shader("VertexShader.cso");
	D3D12Shader pixelShader = D3D12Shader("PixelShader.cso");
	CreateRootSignature();
	pso.Create(vertecies, rootSignature, vertexShader, pixelShader);
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
	cmdList->SetGraphicsRootSignature(rootSignature);

	float color[] = { 1.0f, 1.0f, 0.0f };
	cmdList->SetGraphicsRoot32BitConstants(0, 3, color, 0);
	//cmdList->SetGraphicsRootDescriptorTable(1, descHeap->GetGPUDescriptorHandleForHeapStart());
	//cmdList->SetDescriptorHeaps(1, &descHeap);
}

void D3D12Renderer::Draw(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	ClearBackBuffer(cmdList);

	vertecies.Set(cmdList);

	pso.Set(cmdList);

	SetShaderParams(cmdList);

	SetViewport(cmdList);
	SetBackBufferRTV(cmdList);

	cmdList->DrawInstanced(vertecies.vertexBufferView.SizeInBytes / vertecies.vertexBufferView.StrideInBytes, 1, 0, 0);
}

void D3D12Renderer::Render()
{
	//Begin Drawing
	auto cmdList = D3D12Context::Get().InitializeCommandList();

	//Draw To Window
	D3D12Window::Get().BeginFrame(cmdList);

	Draw(cmdList);

	D3D12Window::Get().EndFrame(cmdList);

	//Finish Drawing & Present
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