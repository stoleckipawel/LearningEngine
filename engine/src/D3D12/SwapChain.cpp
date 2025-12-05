#include "Core/PCH.h"
#include "D3D12/SwapChain.h"
#include "Core/Window.h"
#include "D3D12/RHI.h"
#include "D3D12/DescriptorHeapManager.h"

SwapChain GSwapChain;


// Initializes the swap chain and creates render target views
void SwapChain::Initialize()	
{
	// Create swap chain description
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = GWindow.GetWidth();
	swapChainDesc.Height = GWindow.GetHeight();
	swapChainDesc.Format = m_backBufferFormat;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = NumFramesInFlight;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	// Keep flags minimal and consistent; avoid tearing until explicitly supported
	swapChainDesc.Flags = 0;

	// Create fullscreen swap chain description
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullsceenDesc{};
	swapChainFullsceenDesc.Windowed = true;

	// Create the swap chain for the window
	ComPointer<IDXGISwapChain1> swapChain;
	ThrowIfFailed(
		GRHI.DxgiFactory->CreateSwapChainForHwnd(
			GRHI.CmdQueue,
			GWindow.WindowHWND,
			&swapChainDesc,
			&swapChainFullsceenDesc,
			nullptr,
			&swapChain),
		"Failed To Create Swap Chain for HWND"
	);

	// Query for IDXGISwapChain3 interface
	ThrowIfFailed(swapChain.QueryInterface(m_swapChain), "Failed to Query Swap Chain Interface");

	// Initialize current back buffer index
	m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create render target views for all buffers
	CreateRenderTargetViews();
}


// Clears the current render target view with a solid color
void SwapChain::Clear()
{
	float clearColor[4] = { 1.0f, 0.05f, 0.05f, 1.0f };
	GRHI.GetCommandListScene()->ClearRenderTargetView(GSwapChain.GetCPUHandle(), clearColor, 0, nullptr);
}


// Resizes the swap chain buffers and recreates render target views
void SwapChain::Resize()
{
	ReleaseBuffers();

	m_swapChain->ResizeBuffers(
		NumFramesInFlight,
		GWindow.GetWidth(),
		GWindow.GetHeight(),
		DXGI_FORMAT_UNKNOWN,
		0
	);

	CreateRenderTargetViews();

	// Update back buffer index to the swapchain's current buffer
	m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
}


// Returns the CPU descriptor handle for the specified buffer index
D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCPUHandle(UINT index)
{
	return GDescriptorHeapManager.GetRenderTargetViewHeap().GetCPUHandle(index);
}


// Returns the CPU descriptor handle for the current back buffer
D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCPUHandle()
{
	return GetCPUHandle(m_currentBufferIndex);
}


// Creates render target views for all swap chain buffers
void SwapChain::CreateRenderTargetViews()
{
	for (UINT i = 0; i < NumFramesInFlight; i++)
	{
		// Get the buffer resource from the swap chain
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i])), "Failed To get Swapchain Buffer!");

		// Describe the render target view
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = m_backBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		// Create the render target view
		GRHI.Device->CreateRenderTargetView(m_buffers[i].Get(), &rtvDesc, GetCPUHandle(i));
	}
}


// Returns the default viewport for rendering
D3D12_VIEWPORT SwapChain::GetDefaultViewport()
{
	D3D12_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = float(GWindow.GetWidth());
	vp.Height = float(GWindow.GetHeight());
	// Reversed-Z: map near to 1.0 and far to 0.0 for better precision
	vp.MinDepth = 1.0f;
	vp.MaxDepth = 0.0f;
	return vp;
}


// Returns the default scissor rectangle for rendering
D3D12_RECT SwapChain::GetDefaultScissorRect()
{
	D3D12_RECT scissorRect;
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = GWindow.GetWidth();
	scissorRect.bottom = GWindow.GetHeight();
	return scissorRect;
}


// Presents the current back buffer to the screen
void SwapChain::Present()
{
	ThrowIfFailed(m_swapChain->Present(1, 0), "Failed to Present Swap Chain");
}


// Sets the current buffer to render target state
void SwapChain::SetRenderTargetState()
{
	GRHI.SetBarrier(
		m_buffers[m_currentBufferIndex],
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
}


// Sets the current buffer to present state
void SwapChain::SetPresentState()
{
	GRHI.SetBarrier(
		m_buffers[m_currentBufferIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
}


// Releases all buffer resources
void SwapChain::ReleaseBuffers()
{
	for (UINT i = 0; i < NumFramesInFlight; i++)
	{
		m_buffers[i].Release();
	}
}


// Releases all resources associated with the swap chain
void SwapChain::Shutdown()
{
	ReleaseBuffers();
	m_swapChain.Release();
}