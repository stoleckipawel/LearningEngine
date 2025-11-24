#include "SwapChain.h"
#include "Window.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"

SwapChain GSwapChain;

void SwapChain::Initialize()	
{
	//Create Swap Chain
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		{
			swapChainDesc.Width = GWindow.GetWidth();
			swapChainDesc.Height = GWindow.GetHeight();
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = NumFramesInFlight;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullsceenDesc{};
		{
			swapChainFullsceenDesc.Windowed = true;
		}

		ComPointer<IDXGISwapChain1> swapChain;
		ThrowIfFailed(GRHI.DxgiFactory->CreateSwapChainForHwnd(GRHI.CmdQueue, GWindow.WindowHWND, &swapChainDesc, &swapChainFullsceenDesc, nullptr, &swapChain), " Failed To Create Swap Chain for HWND");
		
		ThrowIfFailed(swapChain.QueryInterface(m_swapChain), "Failed to Query Swap Chain Interface");
	}

	CreateRenderTargetViews();
}

void SwapChain::Clear()
{
	float clearColor[4] = { 255.0f, 0.5f, 0.5f, 1.0f };
	GRHI.GetCommandList()->ClearRenderTargetView(GSwapChain.GetCPUHandle(), clearColor, 0, nullptr);
}

void SwapChain::Resize()
{
	ReleaseBuffers();

	m_swapChain->ResizeBuffers(NumFramesInFlight,
		GWindow.GetWidth(), GWindow.GetHeight(),
		DXGI_FORMAT_UNKNOWN, 
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

	CreateRenderTargetViews();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCPUHandle(UINT index)
{
	return GDescriptorHeapManager.GetRenderTargetViewHeap().GetCPUHandle(index);
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCPUHandle()
{
	return GetCPUHandle(m_currentBufferIndex);
}

void SwapChain::CreateRenderTargetViews()
{
	//Get Swap Buffers
	for (UINT i = 0; i < NumFramesInFlight; i++)
	{
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i])), "Failed To get Swapchain Buffer!");
		
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		{
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;
		}

		GRHI.Device->CreateRenderTargetView(m_buffers[i].Get(), &rtvDesc, GetCPUHandle(i));
	}
}

D3D12_VIEWPORT SwapChain::GetDefaultViewport()
{
	D3D12_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = float(GWindow.GetWidth());
	vp.Height = float(GWindow.GetHeight());
	vp.MinDepth = 1.0f;
	vp.MaxDepth = 0.0f;
	return vp;
}

D3D12_RECT SwapChain::GetDefaultScissorRect()
{
	D3D12_RECT scissorRect;
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = GWindow.GetWidth();
	scissorRect.bottom = GWindow.GetHeight();
	return scissorRect;
}

void SwapChain::Present()
{
	ThrowIfFailed(m_swapChain->Present(1, 0), "Failed to Present Swap Chain");
}

void SwapChain::SetRenderTargetState()
{
	GRHI.SetBarrier(
		m_buffers[m_currentBufferIndex],
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void SwapChain::SetPresentState()
{
	GRHI.SetBarrier(
		m_buffers[m_currentBufferIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
}

void SwapChain::ReleaseBuffers()
{
	for (UINT i = 0; i < NumFramesInFlight; i++)
	{
		m_buffers[i].Release();
	}
}

void SwapChain::Shutdown()
{
	ReleaseBuffers();
	m_swapChain.Release();
}