
#include "PCH.h"
#include "D3D12DebugLayer.h"
#include "D3D12Rhi.h"

#if ENGINE_GPU_VALIDATION

D3D12DebugLayer& D3D12DebugLayer::Get() noexcept
{
	static D3D12DebugLayer instance;
	return instance;
}

// Initializes the Direct3D 12 and DXGI debug layers
void D3D12DebugLayer::Initialize()
{
	if (m_bInitialized)
		return;

	// Enable D3D12 and DXGI debug layers for validation and leak tracking.
	InitD3D12Debug();
	InitDXGIDebug();
	m_bInitialized = true;
}

// Initializes InfoQueue debugging after device creation.
void D3D12DebugLayer::InitializeInfoQueue()
{
	// Configure InfoQueue only when device supports it.
	ConfigureInfoQueue();  // Set break on error/warning/corruption
	ApplyInfoQueueFilters();
}

// Enables the D3D12 debug layer for validation and error reporting.
void D3D12DebugLayer::InitD3D12Debug()
{
	CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(m_d3d12Debug.ReleaseAndGetAddressOf())));
	m_d3d12Debug->EnableDebugLayer();
}

// Enables the DXGI debug layer for leak tracking and live object reporting.
void D3D12DebugLayer::InitDXGIDebug()
{
	CHECK(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_dxgiDebug.ReleaseAndGetAddressOf())));
	m_dxgiDebug->EnableLeakTrackingForThread();
}

// Configures D3D12 InfoQueue to break on error, corruption, and warning messages.
void D3D12DebugLayer::ConfigureInfoQueue()
{
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(GD3D12Rhi.GetDevice()->QueryInterface(IID_PPV_ARGS(infoQueue.ReleaseAndGetAddressOf()))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}
}

// Applies filters to suppress noisy or known-issue messages in the InfoQueue.
void D3D12DebugLayer::ApplyInfoQueueFilters()
{
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(GD3D12Rhi.GetDevice()->QueryInterface(IID_PPV_ARGS(infoQueue.ReleaseAndGetAddressOf()))))
	{
		// Suppress known noisy message id (if present). Keep list small and explicit.
		D3D12_MESSAGE_ID disabledMessages[] = {
		    static_cast<D3D12_MESSAGE_ID>(1424)  // FENCE_ZERO_WAIT (SDK noise)
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = static_cast<UINT>(std::size(disabledMessages));
		filter.DenyList.pIDList = disabledMessages;
		infoQueue->AddStorageFilterEntries(&filter);
	}
}

// Shuts down the debug layers and reports live objects.
// Call before device destruction to catch leaks and report live objects.
void D3D12DebugLayer::Shutdown()
{
	if (!m_bInitialized)
		return;

	ReportLiveDXGIObjects();
	m_dxgiDebug.Reset();
	m_d3d12Debug.Reset();
	m_bInitialized = false;
}

// Reports live D3D12 device objects (must be called before device is Reset).
void D3D12DebugLayer::ReportLiveDeviceObjects()
{
	#if ENGINE_REPORT_LIVE_OBJECTS
	ComPtr<ID3D12DebugDevice> debugDevice;
	if (GD3D12Rhi.GetDevice() && SUCCEEDED(GD3D12Rhi.GetDevice()->QueryInterface(IID_PPV_ARGS(debugDevice.ReleaseAndGetAddressOf()))))
	{
		OutputDebugStringW(L"D3D12 Live Device Objects (detail + summary):\n");
		debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY);
	}
	#endif
}

// Reports DXGI live objects (factory, adapters, swapchains).
void D3D12DebugLayer::ReportLiveDXGIObjects()
{
	#if ENGINE_REPORT_LIVE_OBJECTS
	if (m_dxgiDebug)
	{
		OutputDebugStringW(L"DXGI Live Objects (all flags):\n");
		m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
	}
	#endif
}
#endif
