#pragma once
#include "../Vendor/Windows/WinInclude.h"


// DebugLayer manages Direct3D 12 and DXGI debug layers for diagnostics and leak tracking (debug builds only).
class DebugLayer
{
public:
	// Initializes the debug layers
	void Initialize();
	// Shuts down the debug layers and reports live objects
	void Shutdown();
private:
#if defined(_DEBUG)
	ComPointer<ID3D12Debug> m_d3d12Debug;   // D3D12 debug interface
	ComPointer<IDXGIDebug1> m_dxgiDebug;    // DXGI debug interface
#endif
};


// Global debug layer instance
extern DebugLayer GDebugLayer;