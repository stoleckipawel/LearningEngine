#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <array>
#include "D3D12ConstantBufferData.h"
#include "D3D12ConstantBuffer.h"
#include "Primitive.h"

//------------------------------------------------------------------------------
// ConstantBufferManager
//------------------------------------------------------------------------------
// Manages constant buffer updates with proper GPU/CPU synchronization.
//
// Architecture:
//   - Per-Frame/Per-View CBs: Use persistent ConstantBuffer<T> instances
//     (one per frame-in-flight). These are updated once per frame and bound
//     to root CBV slots. Simple and efficient for low-frequency updates.
//
//   - Per-Object CBs: Use FrameResourceManager's linear allocator to
//     suballocate per-draw. 
//
// Binding Pattern:
//   - Per-Frame/Per-View: Bound once per frame via GetXXXGpuAddress()
//   - Per-Object: UpdatePerObjectXXX() returns a unique GPU VA per call
//
// Thread Safety:
//   - Per-object allocations are thread-safe (atomic linear allocator)
//   - Per-frame/per-view updates should be called from main thread
//------------------------------------------------------------------------------

class D3D12ConstantBufferManager
{
public:
    /// Initialize all constant buffers.
    void Initialize();

    /// Cleanup resources.
    void Shutdown();

    // -------------------------------------------------------------------------
    // GPU Address Accessors (for binding root CBVs)
    // -------------------------------------------------------------------------

    /// Get GPU address of current frame's per-frame CB.
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetPerFrameGpuAddress() const;

    /// Get GPU address of current frame's per-view CB.
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetPerViewGpuAddress() const;

    // -------------------------------------------------------------------------
    // Update Methods
    // -------------------------------------------------------------------------

    /// Update per-frame constant buffer. Call once per frame.
    void UpdatePerFrame();

    /// Update per-view constant buffer. Call once per camera/view.
    void UpdatePerView();

    /// Update per-object VS constant buffer for a primitive.
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UpdatePerObjectVS(const Primitive& primitive);

    /// Update per-object PS constant buffer (material data).
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UpdatePerObjectPS();
private:
    // Per-Frame constant buffers (persistent, one per frame-in-flight)
    std::unique_ptr<D3D12ConstantBuffer<PerFrameConstantBufferData>> m_PerFrameCB[EngineSettings::FramesInFlight];

    // Per-View constant buffers (persistent, one per frame-in-flight)
    std::unique_ptr<D3D12ConstantBuffer<PerViewConstantBufferData>> m_PerViewCB[EngineSettings::FramesInFlight];

};

extern D3D12ConstantBufferManager GD3D12ConstantBufferManager;