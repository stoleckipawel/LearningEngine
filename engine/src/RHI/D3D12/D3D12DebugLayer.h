#pragma once

#ifdef ENGINE_GPU_VALIDATION
#include <wrl/client.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

// DebugLayer manages Direct3D12 / DXGI debug helpers.
// Purpose: enable/disable SDK validation, configure the ID3D12InfoQueue filters,
// and report live objects at shutdown to catch leaks. This class is only
// available when `ENGINE_GPU_VALIDATION` is enabled (typically debug builds).
class D3D12DebugLayer
{
  public:
	D3D12DebugLayer() = default;
	~D3D12DebugLayer() = default;

	D3D12DebugLayer(const D3D12DebugLayer&) = delete;
	D3D12DebugLayer& operator=(const D3D12DebugLayer&) = delete;

	// Initialize debug layers. Safe to call multiple times (idempotent).
	void Initialize();

	// After device creation, initialize InfoQueue filters for the created device.
	void InitializeInfoQueue();

	// Shutdown debug layers and optionally report live objects. Safe to call multiple times.
	void Shutdown();

	// Helpers to explicitly report live objects. Call before releasing the device.
	void ReportLiveDeviceObjects();
	void ReportLiveDXGIObjects();

  private:
	void InitD3D12Debug();
	void InitDXGIDebug();
	void ConfigureInfoQueue();
	void ApplyInfoQueueFilters();

  private:
	ComPtr<ID3D12Debug> m_d3d12Debug;  // D3D12 debug interface
	ComPtr<IDXGIDebug1> m_dxgiDebug;  // DXGI debug interface
	bool m_bInitialized = false;  // guard to make Initialize/Shutdown idempotent
};

// Global debug layer instance
extern D3D12DebugLayer GD3D12DebugLayer;
#endif
