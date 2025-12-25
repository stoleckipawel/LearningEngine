#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <array>
#include <cassert>
#include "LinearAllocator.h"
#include "EngineConfig.h"
#include "D3D12Rhi.h"

//------------------------------------------------------------------------------
// FrameResource
//------------------------------------------------------------------------------
// Represents all GPU resources associated with a single frame-in-flight.
// This includes the linear allocator for dynamic constant buffers and the
// fence value used to track when the GPU has finished processing this frame.
//
// The frame resource pattern is fundamental to D3D12 programming:
//   - Each frame has its own set of mutable resources
//   - We track fence values per-frame to know when GPU is done
//   - Only reset/reuse a frame's resources after its fence is signaled
//
// This prevents the classic CPU/GPU race condition where CPU overwrites
// data that GPU is still reading.
//------------------------------------------------------------------------------

struct D3D12FrameResource
{
	LinearAllocator CbAllocator;  // Per-frame CB ring buffer
	uint64_t FenceValue = 0;  // Fence value when this frame was submitted
	uint32_t FrameIndex = 0;  // Debug: which frame index this represents

	void Initialize(uint64_t allocatorCapacity, uint32_t frameIdx)
	{
		FrameIndex = frameIdx;
		FenceValue = 0;

		wchar_t name[64];
		swprintf_s(name, L"FrameAllocator_%u", frameIdx);
		CbAllocator.Initialize(allocatorCapacity, name);
	}

	void Shutdown() { CbAllocator.Shutdown(); }

	// Reset allocator for new frame. Only call after fence confirms GPU completion.
	void Reset() { CbAllocator.Reset(); }
};

//------------------------------------------------------------------------------
// FrameResourceManager
//------------------------------------------------------------------------------
// Manages the ring of FrameResource instances, one per frame-in-flight.
// Handles synchronization between CPU and GPU to prevent data races.
//
// Synchronization Model:
//   1. BeginFrame(): Wait for the oldest frame's fence, then reset its allocator
//   2. AllocateXXX(): Allocate from current frame's linear allocator
//   3. EndFrame(): Signal fence with current frame's fence value
//   4. Advance frame index (wraps around)
//
// This guarantees that when we reuse a frame's allocator (after FramesInFlight
// frames), the GPU has definitely finished reading from it.
//
// Capacity Planning:
//   Default: 4MB per frame (supports ~16k draw calls with 256-byte CBs)
//   For very large scenes, increase to 8-16MB or implement dynamic growth.
//------------------------------------------------------------------------------

class D3D12FrameResourceManager
{
  public:
	// Default capacity: 4MB per frame (16384 draws × 256 bytes)
	static constexpr uint64_t DefaultCapacityPerFrame = 4 * 1024 * 1024;

	D3D12FrameResourceManager() = default;
	~D3D12FrameResourceManager() { Shutdown(); }

	// Non-copyable
	D3D12FrameResourceManager(const D3D12FrameResourceManager&) = delete;
	D3D12FrameResourceManager& operator=(const D3D12FrameResourceManager&) = delete;

	//--------------------------------------------------------------------------
	// Initialization
	//--------------------------------------------------------------------------

	// Initialize all frame resources.
	// @param capacityPerFrame Allocator capacity per frame (default 4MB).
	void Initialize(uint64_t capacityPerFrame = DefaultCapacityPerFrame)
	{
		m_CapacityPerFrame = capacityPerFrame;

		for (uint32_t i = 0; i < EngineSettings::FramesInFlight; ++i)
		{
			m_FrameResources[i].Initialize(capacityPerFrame, i);
		}

		m_CurrentFrameIndex = 0;
		m_bInitialized = true;
	}

	// Shutdown and release all resources.
	void Shutdown()
	{
		if (!m_bInitialized)
			return;

		for (auto& frame : m_FrameResources)
		{
			frame.Shutdown();
		}

		m_bInitialized = false;
	}

	//--------------------------------------------------------------------------
	// Frame Lifecycle
	//--------------------------------------------------------------------------

	// Begin a new frame. Waits for GPU if necessary, resets allocator.
	void BeginFrame(HANDLE fenceEvent, uint32_t frameIndex)
	{
		assert(m_bInitialized);

		m_CurrentFrameIndex = frameIndex;
		D3D12FrameResource& frame = m_FrameResources[frameIndex];

		// Wait for GPU to finish with this frame's resources before reusing
		// This is the critical synchronization point that prevents races
		const uint64_t completedFence = GD3D12Rhi.GetFence()->GetCompletedValue();
		if (completedFence < frame.FenceValue)
		{
			// GPU hasn't finished with this frame yet - must wait
			HRESULT hr = GD3D12Rhi.GetFence()->SetEventOnCompletion(frame.FenceValue, fenceEvent);
			if (SUCCEEDED(hr))
			{
				WaitForSingleObject(fenceEvent, INFINITE);
			}
		}

		// Now safe to reset - GPU is done with this frame's data
		frame.Reset();
	}

	// Record fence value for current frame. Call after ExecuteCommandLists.
	// fenceValue The fence value that was signaled for this frame.
	void EndFrame(uint64_t fenceValue) { m_FrameResources[m_CurrentFrameIndex].FenceValue = fenceValue; }

	//--------------------------------------------------------------------------
	// Allocation
	//--------------------------------------------------------------------------

	// Get the current frame's linear allocator.
	[[nodiscard]] LinearAllocator& GetCurrentAllocator() { return m_FrameResources[m_CurrentFrameIndex].CbAllocator; }

	// Allocate from current frame's allocator.
	[[nodiscard]] LinearAllocation Allocate(uint64_t size, uint64_t alignment = 256)
	{
		return GetCurrentAllocator().Allocate(size, alignment);
	}

	// Allocate and copy data, return GPU address for CBV binding.
	template <typename T> [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS AllocateConstantBuffer(const T& data)
	{
		return GetCurrentAllocator().AllocateAndCopy(data);
	}

	//--------------------------------------------------------------------------
	// Diagnostics
	//--------------------------------------------------------------------------

	// Get current frame's allocator usage percentage.
	[[nodiscard]] float GetCurrentUsagePercent() const { return m_FrameResources[m_CurrentFrameIndex].CbAllocator.GetUsagePercent(); }

	// Get high water mark across all frames (for capacity tuning).
	[[nodiscard]] uint64_t GetMaxHighWaterMark() const
	{
		uint64_t maxHwm = 0;
		for (const auto& frame : m_FrameResources)
		{
			maxHwm = (std::max) (maxHwm, frame.CbAllocator.GetHighWaterMark());
		}
		return maxHwm;
	}

	// Get capacity per frame.
	[[nodiscard]] uint64_t GetCapacityPerFrame() const { return m_CapacityPerFrame; }

	// Check if initialized.
	[[nodiscard]] bool IsInitialized() const { return m_bInitialized; }

  private:
	std::array<D3D12FrameResource, EngineSettings::FramesInFlight> m_FrameResources;
	uint64_t m_CapacityPerFrame = DefaultCapacityPerFrame;
	uint32_t m_CurrentFrameIndex = 0;
	bool m_bInitialized = false;
};

// Global frame resource manager
extern D3D12FrameResourceManager GD3D12FrameResourceManager;
