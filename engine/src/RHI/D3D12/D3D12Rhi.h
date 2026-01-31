// =============================================================================
// D3D12Rhi.h — Direct3D 12 Rendering Hardware Interface
// =============================================================================
//
// Core RHI layer managing D3D12 device, command queues, allocators, and GPU
// synchronization. Provides a lightweight abstraction over Direct3D 12 with
// minimal COM reference counting overhead.
//
// USAGE:
//   D3D12Rhi rhi;                        // Creates device, queues, fences
//   rhi.ResetCommandAllocator();         // Begin frame
//   rhi.ResetCommandList();
//   // ... record commands ...
//   rhi.CloseCommandList();
//   rhi.ExecuteCommandList();
//   rhi.Signal();                        // End frame
//   rhi.WaitForGPU();                    // Sync before shutdown
//   // destructor releases all resources
//
// DESIGN:
//   - RAII: constructor initializes, destructor cleans up
//   - Getters return const& to internal ComPtr to avoid refcount churn
//   - Per-frame command allocators for FramesInFlight buffering
//   - Fence-based GPU synchronization with per-frame tracking
//
// NOTES:
//   - Owned by Renderer, passed by reference to dependent classes
//   - Requires D3D_FEATURE_LEVEL_12_1 and Shader Model 6.0
//   - Debug layer enabled when ENGINE_GPU_VALIDATION is defined
//
// =============================================================================

#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>
#include "EngineConfig.h"

using Microsoft::WRL::ComPtr;

#ifdef ENGINE_GPU_VALIDATION
class D3D12DebugLayer;
#endif

// =============================================================================
// D3D12Rhi
// =============================================================================

class D3D12Rhi final
{
  public:
	// Constructs and initializes device, command queue, allocators, and fences.
	explicit D3D12Rhi(bool requireDXRSupport = false) noexcept;

	// Releases all D3D12 resources.
	~D3D12Rhi() noexcept;

	D3D12Rhi(const D3D12Rhi&) = delete;
	D3D12Rhi& operator=(const D3D12Rhi&) = delete;
	D3D12Rhi(D3D12Rhi&&) = delete;
	D3D12Rhi& operator=(D3D12Rhi&&) = delete;

	// =========================================================================
	// Command Recording
	// =========================================================================

	// Resets the command allocator for the specified frame. Call at frame start.
	void ResetCommandAllocator(uint32_t frameInFlightIndex) noexcept;

	// Resets and reopens the command list for recording.
	void ResetCommandList(uint32_t frameInFlightIndex) noexcept;

	// Closes the command list. Must be called before ExecuteCommandList().
	void CloseCommandList(uint32_t frameInFlightIndex) noexcept;

	// Submits the closed command list to the GPU queue.
	void ExecuteCommandList(uint32_t frameInFlightIndex) noexcept;

	// Records a resource barrier for state transition.
	void SetBarrier(
	    uint32_t frameInFlightIndex,
	    ID3D12Resource* Resource,
	    D3D12_RESOURCE_STATES StateBefore,
	    D3D12_RESOURCE_STATES StateAfter) noexcept;

	// Sets the current frame index for convenience methods.
	void SetCurrentFrameIndex(uint32_t frameInFlightIndex) noexcept { m_CurrentFrameIndex = frameInFlightIndex; }
	[[nodiscard]] uint32_t GetCurrentFrameIndex() const noexcept { return m_CurrentFrameIndex; }

	// Convenience overloads using current frame index
	void CloseCommandList() noexcept { CloseCommandList(m_CurrentFrameIndex); }
	void ExecuteCommandList() noexcept { ExecuteCommandList(m_CurrentFrameIndex); }
	void SetBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) noexcept
	{
		SetBarrier(m_CurrentFrameIndex, Resource, StateBefore, StateAfter);
	}
	[[nodiscard]] const ComPtr<ID3D12GraphicsCommandList7>& GetCommandList() const noexcept { return m_CmdList[m_CurrentFrameIndex]; }

	// =========================================================================
	// Synchronization
	// =========================================================================

	// Signals the fence with the next value. Call at end of frame.
	void Signal(uint32_t frameInFlightIndex) noexcept;

	// Blocks CPU until GPU completes work for specified frame.
	void WaitForGPU(uint32_t frameInFlightIndex) noexcept;

	// Signal and wait (convenience for shutdown/resize).
	void Flush() noexcept;

	// =========================================================================
	// Device Capabilities
	// =========================================================================

	// Validates Shader Model 6.0 support. Fatals if unsupported.
	void CheckShaderModel6Support() const noexcept;

	// =========================================================================
	// D3D12-Specific Accessors
	// =========================================================================

	[[nodiscard]] const ComPtr<IDXGIFactory7>& GetDxgiFactory() const noexcept { return m_DxgiFactory; }
	[[nodiscard]] const ComPtr<IDXGIAdapter1>& GetAdapter() const noexcept { return m_Adapter; }
	[[nodiscard]] const ComPtr<ID3D12Device10>& GetDevice() const noexcept { return m_Device; }
	[[nodiscard]] const ComPtr<ID3D12CommandQueue>& GetCommandQueue() const noexcept { return m_CmdQueue; }
	[[nodiscard]] const ComPtr<ID3D12CommandAllocator>& GetCommandAllocator(uint32_t FrameInFlightIndex) const noexcept
	{
		return m_CmdAllocator[FrameInFlightIndex];
	}
	[[nodiscard]] const ComPtr<ID3D12GraphicsCommandList7>& GetCommandList(uint32_t FrameInFlightIndex) const noexcept
	{
		return m_CmdList[FrameInFlightIndex];
	}
	[[nodiscard]] const ComPtr<ID3D12Fence1>& GetFence() const noexcept { return m_Fence; }

	// =========================================================================
	// D3D12-Specific Fence Management
	// =========================================================================

	[[nodiscard]] uint64_t GetFenceValueForFrame(uint32_t FrameInFlightIndex) const noexcept { return m_FenceValues[FrameInFlightIndex]; }
	void SetFenceValueForFrame(uint32_t FrameInFlightIndex, uint64_t value) noexcept { m_FenceValues[FrameInFlightIndex] = value; }
	[[nodiscard]] HANDLE GetFenceEvent() const noexcept { return m_FenceEvent; }
	[[nodiscard]] uint64_t GetNextFenceValue() const noexcept { return m_NextFenceValue; }
	void SetNextFenceValue(uint64_t value) noexcept { m_NextFenceValue = value; }

  private:
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

#ifdef ENGINE_GPU_VALIDATION
	std::unique_ptr<D3D12DebugLayer> m_debugLayer;
#endif

	// -------------------------------------------------------------------------
	// D3D12 Resources
	// -------------------------------------------------------------------------

	ComPtr<IDXGIFactory7> m_DxgiFactory = nullptr;
	ComPtr<IDXGIAdapter1> m_Adapter = nullptr;
	ComPtr<ID3D12Device10> m_Device = nullptr;
	ComPtr<ID3D12CommandQueue> m_CmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_CmdAllocator[EngineSettings::FramesInFlight] = {};
	ComPtr<ID3D12GraphicsCommandList7> m_CmdList[EngineSettings::FramesInFlight] = {};
	uint32_t m_CurrentFrameIndex = 0;  // Tracks current frame for methods that don't take frame index

	// -------------------------------------------------------------------------
	// Synchronization State
	// -------------------------------------------------------------------------

	uint64_t m_FenceValues[EngineSettings::FramesInFlight] = {0};  // per-frame fence values
	uint64_t m_NextFenceValue = 1;                                 // monotonically increasing
	ComPtr<ID3D12Fence1> m_Fence = nullptr;
	HANDLE m_FenceEvent = nullptr;
	D3D_FEATURE_LEVEL m_DesiredD3DFeatureLevel = D3D_FEATURE_LEVEL_12_1;
};
