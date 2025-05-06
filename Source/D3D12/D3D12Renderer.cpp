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

void D3D12Renderer::Shutdown()
{
}

std::vector<D3D12_INPUT_ELEMENT_DESC> D3D12Renderer::GetVertexLayout()
{
	return
	{
		{ "Position",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,                          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TexCoord",  0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 2,           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}
