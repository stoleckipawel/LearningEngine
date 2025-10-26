#include "DebugLayer.h"

DebugLayer GDebugLayer;

void DebugLayer::Initialize()
{
#if defined(_DEBUG)
	//Init D3D12 Debug Layer
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug)), "Failed To Initialize D3D12 Debug Interface.");
	m_d3d12Debug->EnableDebugLayer();

	//Init DXGI Debug Layer
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)), "Failed To Initialize DXGI Debug Layer.");
	m_dxgiDebug->EnableLeakTrackingForThread();
#endif
}

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
