#pragma once

// DebugLayer manages Direct3D 12 and DXGI debug layers for diagnostics and leak tracking (debug builds only).
class DebugLayer
{
public:
	// Initializes the debug layers
	void Initialize();
	void InitializeInfoQueue();	
	// Shuts down the debug layers and reports live objects
	void Shutdown();
	// Reports D3D12 live device objects with details (call before releasing device)
	void ReportLiveDeviceObjects();
	// Reports DXGI live objects (factory, adapters, swapchains)
	void ReportLiveDXGIObjects();
private:
	void InitD3D12Debug();
	void InitDXGIDebug();
	void ConfigureInfoQueue();
	void ApplyInfoQueueFilters();
private:
	ComPointer<ID3D12Debug> m_d3d12Debug;   // D3D12 debug interface
	ComPointer<IDXGIDebug1> m_dxgiDebug;    // DXGI debug interface
};


// Global debug layer instance
extern DebugLayer GDebugLayer;