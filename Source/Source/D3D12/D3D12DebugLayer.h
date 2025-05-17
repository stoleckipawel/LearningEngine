#pragma once
#include "../Vendor/Windows/WinInclude.h"
;

class D3D12DebugLayer
{
public:
	bool Initialize();
	void Shutdown();

private:
#if defined(_DEBUG)
	ComPointer<ID3D12Debug> m_d3d12Debug;
	ComPointer<IDXGIDebug1> m_dxgiDebug;
#endif

	//Singleton pattern to ensure only one instance of the debug layer exists
public:
	D3D12DebugLayer(const D3D12DebugLayer&) = delete;
	D3D12DebugLayer& operator=(const D3D12DebugLayer&) = delete;

	inline static D3D12DebugLayer& Get()
	{
		static D3D12DebugLayer instance;
		return instance;
	}
private:
	D3D12DebugLayer() = default;
};