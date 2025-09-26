#pragma once
#include "../Vendor/Windows/WinInclude.h"

class FDebugLayer
{
public:
	void Initialize();
	void Shutdown();
private:
#if defined(_DEBUG)
	ComPointer<ID3D12Debug> m_d3d12Debug;
	ComPointer<IDXGIDebug1> m_dxgiDebug;
#endif
};

extern FDebugLayer GDebugLayer;