#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <atomic>
#include <cassert>
#include <stdexcept>
#include "DebugUtils.h"
#include "D3D12Rhi.h"

using Microsoft::WRL::ComPtr;

//------------------------------------------------------------------------------
// LinearAllocator
//------------------------------------------------------------------------------
// A high-performance per-frame linear (bump) allocator for GPU upload memory.
//
// Design Features:
//   - Single large UPLOAD heap per-frame, mapped once at creation
//   - O(1) allocation via atomic fetch_add (lock-free, thread-safe)
//   - 256-byte alignment for D3D12 constant buffer views
//   - Reset at frame boundary when GPU has finished (no per-alloc overhead)
//   - Returns both CPU pointer (for memcpy) and GPU VA (for binding)
//
// Memory Model:
//   - One LinearAllocator per frame-in-flight
//   - Each frame's allocator is reset only after its fence is signaled
//   - This guarantees GPU has finished reading before CPU overwrites
//
// Usage Pattern:
//   1. BeginFrame() - Reset allocator when fence confirms GPU completion
//   2. Allocate() - Get CPU ptr + GPU VA, write data directly
//   3. Bind GPU VA via SetGraphicsRootConstantBufferView()
//   4. EndFrame() - Signal fence, advance to next frame's allocator
//
// Thread Safety:
//   - Allocate() is fully thread-safe via atomic offset advancement
//   - BeginFrame()/Reset() must be called from main thread only
//------------------------------------------------------------------------------

struct LinearAllocation
{
	void* CpuPtr = nullptr;                    // Write destination
	D3D12_GPU_VIRTUAL_ADDRESS GpuAddress = 0;  // Bind address for CBV
	uint64_t Size = 0;                         // Allocated size (aligned)
	uint64_t Offset = 0;                       // Offset from buffer start
};

class LinearAllocator
{
  public:
	LinearAllocator() = default;
	~LinearAllocator() { Shutdown(); }

	// Non-copyable, non-movable (owns GPU resource)
	LinearAllocator(const LinearAllocator&) = delete;
	LinearAllocator& operator=(const LinearAllocator&) = delete;
	LinearAllocator(LinearAllocator&&) = delete;
	LinearAllocator& operator=(LinearAllocator&&) = delete;

	//--------------------------------------------------------------------------
	// Initialization
	//--------------------------------------------------------------------------

	// Creates the upload buffer with specified capacity.
	void Initialize(uint64_t capacity, const wchar_t* debugName = L"LinearAllocator");

	// Releases all resources. Called automatically by destructor.
	void Shutdown();

	//--------------------------------------------------------------------------
	// Frame Lifecycle
	//--------------------------------------------------------------------------

	// Resets the allocator for a new frame.
	// MUST only be called after confirming GPU has finished with this frame's data.
	// This is typically done via fence synchronization in the frame resource manager.
	void Reset() noexcept;

	//--------------------------------------------------------------------------
	// Allocation (Thread-Safe)
	//--------------------------------------------------------------------------

	// Allocates aligned memory from the linear buffer.
	[[nodiscard]] LinearAllocation Allocate(uint64_t size, uint64_t alignment = 256);

	// Convenience method: allocate, copy data, return GPU address.
	template <typename T> [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS AllocateAndCopy(const T& data)
	{
		static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

		LinearAllocation alloc = Allocate(sizeof(T), 256);
		std::memcpy(alloc.CpuPtr, &data, sizeof(T));
		return alloc.GpuAddress;
	}

	//--------------------------------------------------------------------------
	// Diagnostics
	//--------------------------------------------------------------------------

	// Returns current allocation offset (bytes used this frame).
	[[nodiscard]] uint64_t GetCurrentOffset() const noexcept { return m_Offset.load(std::memory_order_relaxed); }

	// Returns total capacity in bytes.
	[[nodiscard]] uint64_t GetCapacity() const noexcept { return m_Capacity; }

	// Returns peak usage across all frames (for capacity tuning).
	[[nodiscard]] uint64_t GetHighWaterMark() const noexcept { return m_HighWaterMark.load(std::memory_order_relaxed); }

	// Returns percentage of capacity used this frame.
	[[nodiscard]] float GetUsagePercent() const noexcept
	{
		if (m_Capacity == 0)
			return 0.0f;
		return static_cast<float>(GetCurrentOffset()) / static_cast<float>(m_Capacity) * 100.0f;
	}

	// Returns true if allocator is initialized and ready.
	[[nodiscard]] bool IsInitialized() const noexcept { return m_bInitialized; }

  private:
	// Aligns a value up to the specified alignment.
	[[nodiscard]] static constexpr uint64_t AlignUp(uint64_t value, uint64_t alignment) noexcept
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

  private:
	ComPtr<ID3D12Resource> m_Resource;
	uint8_t* m_CpuBase = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuBase = 0;
	uint64_t m_Capacity = 0;
	std::atomic<uint64_t> m_Offset{0};
	std::atomic<uint64_t> m_HighWaterMark{0};
	bool m_bInitialized = false;
};
