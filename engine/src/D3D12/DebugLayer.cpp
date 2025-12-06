
#include "PCH.h"
#include "D3D12/DebugLayer.h"
#include "D3D12/RHI.h"

// Global debug layer instance
DebugLayer GDebugLayer;

// Initializes the Direct3D 12 and DXGI debug layers (only in debug builds)
void DebugLayer::Initialize()
{
#if defined(_DEBUG)
	// Initialize each feature via dedicated functions
	InitD3D12Debug();
	InitDXGIDebug();
#endif
}

// Initialize InfoQueue debugging after device creation
void DebugLayer::InitializeInfoQueue()
{
#if defined(_DEBUG)	
	ConfigureInfoQueue();
	ApplyInfoQueueFilters();
#endif
}

void DebugLayer::InitD3D12Debug()
{
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(m_d3d12Debug.ReleaseAndGetAddressOf())), "Failed To Initialize D3D12 Debug Interface.");
	m_d3d12Debug->EnableDebugLayer();
}

void DebugLayer::InitDXGIDebug()
{
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(m_dxgiDebug.ReleaseAndGetAddressOf())), "Failed To Initialize DXGI Debug Layer.");
	m_dxgiDebug->EnableLeakTrackingForThread();
}

// Configure D3D12 InfoQueue to break on warnings, errors and corruption
void DebugLayer::ConfigureInfoQueue()
{
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(GRHI.GetDevice()->QueryInterface(IID_PPV_ARGS(infoQueue.ReleaseAndGetAddressOf()))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}
}

// Apply filters to suppress noisy/known-issue messages in the InfoQueue
void DebugLayer::ApplyInfoQueueFilters()
{
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(GRHI.GetDevice()->QueryInterface(IID_PPV_ARGS(infoQueue.ReleaseAndGetAddressOf()))))
	{
		//Suppress FENCE_ZERO_WAIT (SDK layer noise/bug); not always present in headers
		const int D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ = 1424;
		D3D12_MESSAGE_ID disabledMessages[] = { (D3D12_MESSAGE_ID)D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = 1;
		filter.DenyList.pIDList = disabledMessages;
		infoQueue->AddStorageFilterEntries(&filter);
	}
}

// Shuts down the debug layers and reports live objects (only in debug builds)
void DebugLayer::Shutdown()
{
#if defined(_DEBUG)
	ReportLiveDXGIObjects();
	m_dxgiDebug.Reset();
	m_d3d12Debug.Reset();
#endif
}

// Reports live D3D12 device objects (must be called before device is Resetd)
void DebugLayer::ReportLiveDeviceObjects()
{
#if defined(_DEBUG) && defined(REPORT_LIVE_OBJECTS)
	Microsoft::WRL::ComPtr<ID3D12DebugDevice> debugDevice;
	if (SUCCEEDED(GRHI.GetDevice()->QueryInterface(IID_PPV_ARGS(debugDevice.ReleaseAndGetAddressOf()))))
	{
		OutputDebugStringW(L"D3D12 Live Device Objects (detail + summary):\n");
		debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_SUMMARY);
	}
#endif
}

// Reports DXGI live objects (factory, adapters, swapchains)
void DebugLayer::ReportLiveDXGIObjects()
{
#if defined(_DEBUG) && defined(REPORT_LIVE_OBJECTS)
	if (m_dxgiDebug)
	{
		OutputDebugStringW(L"DXGI Live Objects (all flags):\n");
		m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
	}
#endif
}
