// ============================================================================
// D3D12DebugLayer.h
// ----------------------------------------------------------------------------
// Manages D3D12 and DXGI debug/validation layers.
//
// USAGE:
//   #ifdef ENGINE_GPU_VALIDATION
//   GD3D12DebugLayer.Initialize();        // Before device creation
//   GD3D12DebugLayer.InitializeInfoQueue(); // After device creation
//   // ... application runs ...
//   GD3D12DebugLayer.Shutdown();          // Reports live objects
//   #endif
//
// DESIGN:
//   - Only available when ENGINE_GPU_VALIDATION is defined (debug builds)
//   - Enables SDK validation layers for catching API misuse
//   - Configures ID3D12InfoQueue filters to control message severity
//   - Reports live D3D12/DXGI objects at shutdown for leak detection
//
// NOTES:
//   - Singleton accessed via GD3D12DebugLayer global reference
//   - Initialize/Shutdown are idempotent (safe to call multiple times)
// ============================================================================

#pragma once

#ifdef ENGINE_GPU_VALIDATION
	#include <wrl/client.h>
	#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class D3D12DebugLayer final
{
  public:
	[[nodiscard]] static D3D12DebugLayer& Get() noexcept;

	D3D12DebugLayer(const D3D12DebugLayer&) = delete;
	D3D12DebugLayer& operator=(const D3D12DebugLayer&) = delete;
	D3D12DebugLayer(D3D12DebugLayer&&) = delete;
	D3D12DebugLayer& operator=(D3D12DebugLayer&&) = delete;

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
	D3D12DebugLayer() = default;
	~D3D12DebugLayer() = default;

	void InitD3D12Debug();
	void InitDXGIDebug();
	void ConfigureInfoQueue();
	void ApplyInfoQueueFilters();

	ComPtr<ID3D12Debug> m_d3d12Debug;  // D3D12 debug interface
	ComPtr<IDXGIDebug1> m_dxgiDebug;   // DXGI debug interface
	bool m_bInitialized = false;       // guard to make Initialize/Shutdown idempotent
};

inline D3D12DebugLayer& GD3D12DebugLayer = D3D12DebugLayer::Get();
#endif
