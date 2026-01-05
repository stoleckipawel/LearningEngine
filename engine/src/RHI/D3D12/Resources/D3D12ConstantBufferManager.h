// ============================================================================
// D3D12ConstantBufferManager.h
// Centralized constant buffer management with GPU/CPU synchronization.
// ----------------------------------------------------------------------------
// USAGE:
//   D3D12ConstantBufferManager::Get().Initialize();
//   auto gpuAddr = D3D12ConstantBufferManager::Get().GetPerFrameGpuAddress();
//   D3D12ConstantBufferManager::Get().UpdatePerFrame(frameData);
//
// DESIGN:
//   Per-Frame/Per-View CBs:
//     Use persistent ConstantBuffer<T> instances (one per frame-in-flight).
//     Updated once per frame, bound to root CBV slots.
//
//   Per-Object CBs:
//     Use FrameResourceManager's linear allocator for suballocation per-draw.
//     UpdatePerObjectXXX() returns a unique GPU VA per call.
//
// NOTES:
//   - Per-object allocations are thread-safe (atomic linear allocator)
//   - Per-frame/per-view updates should be called from main thread
// ============================================================================
#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <array>
#include "D3D12ConstantBufferData.h"
#include "D3D12ConstantBuffer.h"

class D3D12ConstantBufferManager final
{
  public:
	[[nodiscard]] static D3D12ConstantBufferManager& Get() noexcept;

	D3D12ConstantBufferManager(const D3D12ConstantBufferManager&) = delete;
	D3D12ConstantBufferManager& operator=(const D3D12ConstantBufferManager&) = delete;
	D3D12ConstantBufferManager(D3D12ConstantBufferManager&&) = delete;
	D3D12ConstantBufferManager& operator=(D3D12ConstantBufferManager&&) = delete;

	// Initialize all constant buffers.
	void Initialize();

	// Cleanup resources.
	void Shutdown();

	// -------------------------------------------------------------------------
	// GPU Address Accessors (for binding root CBVs)
	// -------------------------------------------------------------------------

	// Get GPU address of current frame's per-frame CB.
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetPerFrameGpuAddress() const;

	// Get GPU address of current frame's per-view CB.
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetPerViewGpuAddress() const;

	// -------------------------------------------------------------------------
	// Update Methods
	// -------------------------------------------------------------------------

	// Update per-frame constant buffer. Call once per frame.
	void UpdatePerFrame();

	// Update per-view constant buffer. Call once per camera/view.
	void UpdatePerView();

	// Update per-object VS constant buffer for a draw.
	// Any system can provide this data (Primitive, SkeletalMesh, etc.) without coupling.
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UpdatePerObjectVS(const PerObjectVSConstantBufferData& data);

	// Update per-object PS constant buffer (material data).
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UpdatePerObjectPS();

  private:
	D3D12ConstantBufferManager() = default;
	~D3D12ConstantBufferManager() = default;

	// Per-Frame constant buffers (persistent, one per frame-in-flight)
	std::unique_ptr<D3D12ConstantBuffer<PerFrameConstantBufferData>> m_PerFrameCB[EngineSettings::FramesInFlight];

	// Per-View constant buffers (persistent, one per frame-in-flight)
	std::unique_ptr<D3D12ConstantBuffer<PerViewConstantBufferData>> m_PerViewCB[EngineSettings::FramesInFlight];
};

inline D3D12ConstantBufferManager& GD3D12ConstantBufferManager = D3D12ConstantBufferManager::Get();