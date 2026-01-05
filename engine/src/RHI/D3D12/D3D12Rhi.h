// =============================================================================
// D3D12Rhi.h — Direct3D 12 Rendering Hardware Interface
// =============================================================================
//
// Core RHI layer managing D3D12 device, command queues, allocators, and GPU
// synchronization. Provides a lightweight abstraction over Direct3D 12 with
// minimal COM reference counting overhead.
//
// USAGE:
//   GD3D12Rhi.Initialize();              // Create device, queues, fences
//   GD3D12Rhi.ResetCommandAllocator();   // Begin frame
//   GD3D12Rhi.ResetCommandList();
//   // ... record commands ...
//   GD3D12Rhi.CloseCommandListScene();
//   GD3D12Rhi.ExecuteCommandList();
//   GD3D12Rhi.Signal();                  // End frame
//   GD3D12Rhi.WaitForGPU();              // Sync before shutdown
//   GD3D12Rhi.Shutdown();
//
// DESIGN:
//   - Getters return const& to internal ComPtr to avoid refcount churn
//   - Per-frame command allocators for FramesInFlight buffering
//   - Fence-based GPU synchronization with per-frame tracking
//
// NOTES:
//   - Singleton accessed via GD3D12Rhi global
//   - Requires D3D_FEATURE_LEVEL_12_1 and Shader Model 6.0
//   - Debug layer enabled when ENGINE_GPU_VALIDATION is defined
//
// =============================================================================

#pragma once

#include "D3D12SwapChain.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// =============================================================================
// D3D12Rhi
// =============================================================================

class D3D12Rhi final
{
  public:
	[[nodiscard]] static D3D12Rhi& Get() noexcept;

	D3D12Rhi(const D3D12Rhi&) = delete;
	D3D12Rhi& operator=(const D3D12Rhi&) = delete;
	D3D12Rhi(D3D12Rhi&&) = delete;
	D3D12Rhi& operator=(D3D12Rhi&&) = delete;

	// =========================================================================
	// Lifecycle
	// =========================================================================

	// Initializes device, command queue, allocators, and fences.
	void Initialize(bool RequireDXRSupport = false) noexcept;

	// Releases all D3D12 resources. Call after final WaitForGPU().
	void Shutdown() noexcept;

	// =========================================================================
	// Command Recording
	// =========================================================================

	// Resets the command allocator for the current frame. Call at frame start.
	void ResetCommandAllocator() noexcept;

	// Resets and reopens the command list for recording.
	void ResetCommandList() noexcept;

	// Closes the command list. Must be called before ExecuteCommandList().
	void CloseCommandListScene() noexcept;

	// Submits the closed command list to the GPU queue.
	void ExecuteCommandList() noexcept;

	// Records a resource barrier for state transition.
	void SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) noexcept;

	// =========================================================================
	// Synchronization
	// =========================================================================

	// Signals the fence with the next value. Call at end of frame.
	void Signal() noexcept;

	// Blocks CPU until GPU completes all pending work.
	void WaitForGPU() noexcept;

	// Signal and wait (convenience for shutdown/resize).
	void Flush() noexcept;

	// =========================================================================
	// Device Capabilities
	// =========================================================================

	// Validates Shader Model 6.0 support. Fatals if unsupported.
	void CheckShaderModel6Support() const noexcept;

	// =========================================================================
	// Accessors
	// =========================================================================

	[[nodiscard]] const ComPtr<IDXGIFactory7>& GetDxgiFactory() const noexcept { return m_DxgiFactory; }
	[[nodiscard]] const ComPtr<IDXGIAdapter1>& GetAdapter() const noexcept { return m_Adapter; }
	[[nodiscard]] const ComPtr<ID3D12Device10>& GetDevice() const noexcept { return m_Device; }
	[[nodiscard]] const ComPtr<ID3D12CommandQueue>& GetCommandQueue() const noexcept { return m_CmdQueue; }
	[[nodiscard]] const ComPtr<ID3D12CommandAllocator>& GetCommandAllocatorScene(UINT FrameInFlightIndex) const noexcept
	{
		return m_CmdAllocatorScene[FrameInFlightIndex];
	}
	[[nodiscard]] const ComPtr<ID3D12GraphicsCommandList7>& GetCommandList() const noexcept { return m_CmdListScene; }
	[[nodiscard]] const ComPtr<ID3D12Fence1>& GetFence() const noexcept { return m_Fence; }

	// =========================================================================
	// Fence Management
	// =========================================================================

	[[nodiscard]] UINT64 GetFenceValueForFrame(UINT FrameInFlightIndex) const noexcept { return m_FenceValues[FrameInFlightIndex]; }
	void SetFenceValueForFrame(UINT FrameInFlightIndex, UINT64 value) noexcept { m_FenceValues[FrameInFlightIndex] = value; }
	[[nodiscard]] HANDLE GetFenceEvent() const noexcept { return m_FenceEvent; }
	[[nodiscard]] UINT64 GetNextFenceValue() const noexcept { return m_NextFenceValue; }
	void SetNextFenceValue(UINT64 value) noexcept { m_NextFenceValue = value; }

  private:
	D3D12Rhi() = default;
	~D3D12Rhi() = default;

	// -------------------------------------------------------------------------
	// Initialization Helpers
	// -------------------------------------------------------------------------

	void SelectAdapter() noexcept;
	void CreateFactory();
	void CreateDevice(bool requireDXRSupport);
	void CreateCommandQueue();
	void CreateCommandAllocators();
	void CreateCommandLists();
	void CreateFenceAndEvent();

	// -------------------------------------------------------------------------
	// D3D12 Resources
	// -------------------------------------------------------------------------

	ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;
	ComPtr<IDXGIAdapter1> m_Adapter = nullptr;
	ComPtr<ID3D12Device10> m_Device = nullptr;
	ComPtr<ID3D12CommandQueue> m_CmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_CmdAllocatorScene[EngineSettings::FramesInFlight] = {};
	ComPtr<ID3D12GraphicsCommandList7> m_CmdListScene = nullptr;

	// -------------------------------------------------------------------------
	// Synchronization State
	// -------------------------------------------------------------------------

	UINT64 m_FenceValues[EngineSettings::FramesInFlight] = {0};  // per-frame fence values
	UINT64 m_NextFenceValue = 1;                                  // monotonically increasing
	ComPtr<ID3D12Fence1> m_Fence = nullptr;
	HANDLE m_FenceEvent = nullptr;
	D3D_FEATURE_LEVEL m_DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
};

inline D3D12Rhi& GD3D12Rhi = D3D12Rhi::Get();
