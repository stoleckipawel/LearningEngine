
#include "PCH.h"
#include "DebugLayer.h"

// Global debug layer instance
DebugLayer GDebugLayer;

// Initializes the Direct3D 12 and DXGI debug layers (only in debug builds)
void DebugLayer::Initialize()
{
#if defined(_DEBUG)
	// Initialize D3D12 debug layer
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug)), "Failed To Initialize D3D12 Debug Interface.");
	m_d3d12Debug->EnableDebugLayer();

	// Initialize DXGI debug layer
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)), "Failed To Initialize DXGI Debug Layer.");
	m_dxgiDebug->EnableLeakTrackingForThread();
#endif
}

// Shuts down the debug layers and reports live objects (only in debug builds)
void DebugLayer::Shutdown()
{
#if defined(_DEBUG)
	if (m_dxgiDebug)
	{
		OutputDebugStringW(L"DXGI Reports Living Device Objects: \n");
		m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
	}

	m_dxgiDebug.Release();
	m_d3d12Debug.Release();
#endif
}
