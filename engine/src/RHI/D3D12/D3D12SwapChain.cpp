#include "PCH.h"
#include "D3D12SwapChain.h"
#include "Window.h"
#include "D3D12Rhi.h"
#include "D3D12DescriptorHeapManager.h"
#include "DebugUtils.h"

D3D12SwapChain GD3D12SwapChain;

// Initializes the swap chain and creates render target views
void D3D12SwapChain::Initialize()
{
	AllocateHandles();
	Create();

	m_swapChain->SetMaximumFrameLatency(EngineSettings::FramesInFlight);
	m_WaitableObject = m_swapChain->GetFrameLatencyWaitableObject();

	// Initialize current frame-in-flight index from swap chain
	UpdateFrameInFlightIndex();

	// Create render target views for all buffers
	CreateRenderTargetViews();
}

void D3D12SwapChain::Create()
{
	// Create swap chain description
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = GWindow.GetWidth();
	swapChainDesc.Height = GWindow.GetHeight();
	swapChainDesc.Format = EngineSettings::BackBufferFormat;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = EngineSettings::FramesInFlight;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = ComputeSwapChainFlags();

	// Create fullscreen swap chain description
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullsceenDesc{};
	swapChainFullsceenDesc.Windowed = true;

	// Create the swap chain for the window
	ComPtr<IDXGISwapChain1> swapChain;
	CHECK(GD3D12Rhi.GetDxgiFactory()->CreateSwapChainForHwnd(
	    GD3D12Rhi.GetCommandQueue().Get(),
	    GWindow.GetHWND(),
	    &swapChainDesc,
	    &swapChainFullsceenDesc,
	    nullptr,
	    &swapChain));

	// Query for IDXGISwapChain3 interface
	CHECK(swapChain.As(&m_swapChain));
}

// Clears the current render target view with a solid color
void D3D12SwapChain::Clear()
{
	float clearColor[4] = {0.0f, 0.00f, 0.00f, 1.0f};
	GD3D12Rhi.GetCommandList()->ClearRenderTargetView(GD3D12SwapChain.GetCPUHandle(), clearColor, 0, nullptr);
}

// Resizes the swap chain buffers and recreates render target views
void D3D12SwapChain::Resize()
{
	ReleaseBuffers();

	m_swapChain->ResizeBuffers(
	    EngineSettings::FramesInFlight,
	    GWindow.GetWidth(),
	    GWindow.GetHeight(),
	    EngineSettings::BackBufferFormat,
	    ComputeSwapChainFlags());

	CreateRenderTargetViews();

	UpdateFrameInFlightIndex();
}

// Returns the CPU descriptor handle for the current back buffer
void D3D12SwapChain::AllocateHandles()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; i++)
	{
		m_rtvHandles[i] = GD3D12DescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

// Creates render target views for all swap chain buffers
void D3D12SwapChain::CreateRenderTargetViews()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; i++)
	{
		// Get the buffer resource from the swap chain
		CHECK(m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_buffers[i].ReleaseAndGetAddressOf())));
		DebugUtils::SetDebugName(m_buffers[i], L"RHI_BackBuffer");

		// Describe the render target view
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = EngineSettings::BackBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		// Create the render target view
		GD3D12Rhi.GetDevice()->CreateRenderTargetView(m_buffers[i].Get(), &rtvDesc, GetCPUHandle(i));
	}
}

UINT D3D12SwapChain::GetAllowTearingFlag() const
{
	BOOL allowTearing = FALSE;

	GD3D12Rhi.GetDxgiFactory()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));

	return (allowTearing == TRUE) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
}

UINT D3D12SwapChain::ComputeSwapChainFlags() const
{
	UINT flags = 0u;
	flags |= GetFrameLatencyWaitableFlag();
	flags |= GetAllowTearingFlag();
	return flags;
}

// Returns the default viewport for rendering
D3D12_VIEWPORT D3D12SwapChain::GetDefaultViewport() const
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
D3D12_RECT D3D12SwapChain::GetDefaultScissorRect() const
{
	D3D12_RECT scissorRect;
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = GWindow.GetWidth();
	scissorRect.bottom = GWindow.GetHeight();
	return scissorRect;
}

// Presents the current back buffer to the screen
void D3D12SwapChain::Present()
{
	// Present according to runtime setting: vsync on -> interval 1, vsync off -> interval 0
	UINT presentInterval = EngineSettings::VSync ? 1u : 0u;
	UINT presentFlags = 0u;
	if (!EngineSettings::VSync)
	{
		// If tearing is supported by the runtime, request it when presenting without vsync.
		BOOL allowTearing = FALSE;
		GD3D12Rhi.GetDxgiFactory()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		presentFlags = (allowTearing == TRUE) ? DXGI_PRESENT_ALLOW_TEARING : 0u;
	}

	// Present the frame
	CHECK(m_swapChain->Present(presentInterval, presentFlags));
}

// Sets the current buffer to render target state
void D3D12SwapChain::SetRenderTargetState()
{
	GD3D12Rhi.SetBarrier(m_buffers[m_frameInFlightIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

// Sets the current buffer to present state
void D3D12SwapChain::SetPresentState()
{
	GD3D12Rhi.SetBarrier(

	    m_buffers[m_frameInFlightIndex].Get(),
	    D3D12_RESOURCE_STATE_RENDER_TARGET,
	    D3D12_RESOURCE_STATE_PRESENT);
}

// Releases all buffer resources
void D3D12SwapChain::ReleaseBuffers()
{
	for (UINT i = 0; i < EngineSettings::FramesInFlight; i++)
	{
		m_buffers[i].Reset();
		if (m_rtvHandles[i].IsValid())
		{
			GD3D12DescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_rtvHandles[i]);
		}
	}
}

// Releases all resources associated with the swap chain
void D3D12SwapChain::Shutdown()
{
	ReleaseBuffers();
	m_swapChain.Reset();
}