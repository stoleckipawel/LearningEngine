// ============================================================================
// D3D12DebugLayer.h
// ----------------------------------------------------------------------------
// Manages D3D12 and DXGI debug/validation layers.
//
// USAGE:
//   // Owned by D3D12Rhi, created before device:
//   m_debugLayer = std::make_unique<D3D12DebugLayer>();
//   // After device creation:
//   m_debugLayer->InitializeInfoQueue(device);
//   // Destructor handles shutdown and live object reporting
//
// DESIGN:
//   - Only available when ENGINE_GPU_VALIDATION is defined (debug builds)
//   - Enables SDK validation layers for catching API misuse
//   - Configures ID3D12InfoQueue filters to control message severity
//   - Reports live D3D12/DXGI objects at shutdown for leak detection
//
// NOTES:
//   - Owned by D3D12Rhi as a unique_ptr member
//   - Must be created before device and destroyed after device
// ============================================================================

#pragma once

#ifdef ENGINE_GPU_VALIDATION
	#include <wrl/client.h>
	#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class D3D12DebugLayer final
{
  public:
	// Constructs and enables debug layers. Call before device creation.
	D3D12DebugLayer();

	// Shuts down debug layers and reports live objects.
	~D3D12DebugLayer() noexcept;

	D3D12DebugLayer(const D3D12DebugLayer&) = delete;
	D3D12DebugLayer& operator=(const D3D12DebugLayer&) = delete;
	D3D12DebugLayer(D3D12DebugLayer&&) = delete;
	D3D12DebugLayer& operator=(D3D12DebugLayer&&) = delete;

	// After device creation, initialize InfoQueue filters for the created device.
	void InitializeInfoQueue(ID3D12Device* device);

	// Reports live D3D12 device objects (call before device Reset).
	void ReportLiveDeviceObjects(ID3D12Device* device);

	// Reports DXGI live objects (factory, adapters, swapchains).
	void ReportLiveDXGIObjects();

  private:
	void InitD3D12Debug();
	void InitDXGIDebug();
	void ConfigureInfoQueue(ID3D12Device* device);
	void ApplyInfoQueueFilters(ID3D12Device* device);

	ComPtr<ID3D12Debug> m_d3d12Debug;  // D3D12 debug interface
	ComPtr<IDXGIDebug1> m_dxgiDebug;   // DXGI debug interface
};

#endif
