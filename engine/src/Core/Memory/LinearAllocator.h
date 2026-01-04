#pragma once

#include "DebugUtils.h"
#include "D3D12Rhi.h"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <d3d12.h>
#include <stdexcept>
#include <wrl/client.h>

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

	// Creates the upload buffer with specified capacity.
	void Initialize(uint64_t capacity, const wchar_t* debugName = L"LinearAllocator");

	// Releases all resources. Called automatically by destructor.
	void Shutdown();

	// Resets the allocator for a new frame.
	void Reset() noexcept;

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
