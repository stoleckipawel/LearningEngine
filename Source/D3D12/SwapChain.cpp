#include "SwapChain.h"
#include "Window.h"
#include "RHI.h"

SwapChain GSwapChain;

void SwapChain::Initialize()	
{
	//Create Swap Chain
	{
		//Describe Swapchain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		{
			swapChainDesc.Width = GWindow.GetWidth();
			swapChainDesc.Height = GWindow.GetHeight();
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = FrameCount;
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
		ThrowIfFailed(GRHI.DxgiFactory->CreateSwapChainForHwnd(GRHI.CmdQueue, GWindow.m_window, &swapChainDesc, &swapChainFullsceenDesc, nullptr, &swapChain), " Failed To Create Swap Chain for HWND");
		
		ThrowIfFailed(swapChain.QueryInterface(m_swapChain), "Failed to Query Swap Chain Interface");
	}

	//Create Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	{
		rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescHeapDesc.NumDescriptors = FrameCount;
		rtvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(GRHI.Device->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&m_rtvHeap)), "Failed to Create Descriptor Heap for Window");

		m_rtvDescriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//Create RenderTarget View & View Handles
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < FrameCount; i++)
		{
			m_rtvHandles[i] = rtvHandle;
			m_rtvHandles[i].ptr += m_rtvDescriptorSize * i;
		}

		CreateRenderTargetViews();
	}
}

void SwapChain::Resize()
{
	ReleaseBuffers();

	m_swapChain->ResizeBuffers(FrameCount,
		GWindow.GetWidth(), GWindow.GetHeight(),
		DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

	CreateRenderTargetViews();
}

void SwapChain::CreateRenderTargetViews()
{
	//Get Swap Buffers
	for (UINT i = 0; i < FrameCount; i++)
	{
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i])), "Failed To get Swapchain Buffer!");
		
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		{
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;
		}

		GRHI.Device->CreateRenderTargetView(m_buffers[i].Get(), &rtvDesc, m_rtvHandles[i]);
	}
}

ID3D12Resource* SwapChain::GetBackBufferResource()
{
	return m_buffers[m_currentBufferIndex];
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

void SwapChain::ReleaseBuffers()
{
	for (UINT i = 0; i < FrameCount; i++)
	{
		m_buffers[i].Release();
	}
}

void SwapChain::Shutdown()
{
	ReleaseBuffers();

	m_rtvHeap.Release();

	m_swapChain.Release();
}