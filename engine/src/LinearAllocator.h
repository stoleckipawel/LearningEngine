#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <atomic>
#include <cassert>
#include <stdexcept>
#include "DebugUtils.h"
#include "RHI.h"

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
    void*                       CpuPtr     = nullptr;  // Write destination
    D3D12_GPU_VIRTUAL_ADDRESS   GpuAddress = 0;        // Bind address for CBV
    uint64_t                    Size       = 0;        // Allocated size (aligned)
    uint64_t                    Offset     = 0;        // Offset from buffer start
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
    
    /// Creates the upload buffer with specified capacity.
    /// @param capacity Total size in bytes (recommend 4-16MB for typical scenes).
    /// @param debugName Optional debug name for GPU debugging tools.
    void Initialize(uint64_t capacity, const wchar_t* debugName = L"LinearAllocator")
    {
        assert(capacity > 0);
        
        m_Capacity = capacity;
        m_Offset.store(0, std::memory_order_relaxed);

        // Create committed UPLOAD resource
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = capacity;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        HRESULT hr = GRHI.GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_Resource));

        if (FAILED(hr))
        {
            throw std::runtime_error("LinearAllocator: Failed to create upload buffer");
        }

        DebugUtils::SetDebugName(m_Resource, debugName);

        // Map once and keep mapped for lifetime (UPLOAD heap allows persistent mapping)
        D3D12_RANGE readRange = { 0, 0 }; // We don't read from this buffer
        hr = m_Resource->Map(0, &readRange, reinterpret_cast<void**>(&m_CpuBase));
        if (FAILED(hr))
        {
            throw std::runtime_error("LinearAllocator: Failed to map upload buffer");
        }

        m_GpuBase = m_Resource->GetGPUVirtualAddress();
        m_Initialized = true;
    }

    /// Releases all resources. Called automatically by destructor.
    void Shutdown()
    {
        if (m_Resource)
        {
            m_Resource->Unmap(0, nullptr);
            m_Resource.Reset();
        }
        m_CpuBase = nullptr;
        m_GpuBase = 0;
        m_Capacity = 0;
        m_Offset.store(0, std::memory_order_relaxed);
        m_Initialized = false;
    }

    //--------------------------------------------------------------------------
    // Frame Lifecycle
    //--------------------------------------------------------------------------

    /// Resets the allocator for a new frame. 
    /// MUST only be called after confirming GPU has finished with this frame's data.
    /// This is typically done via fence synchronization in the frame resource manager.
    void Reset() noexcept
    {
        m_Offset.store(0, std::memory_order_release);
        m_HighWaterMark = 0; // Reset per-frame tracking
    }

    //--------------------------------------------------------------------------
    // Allocation (Thread-Safe)
    //--------------------------------------------------------------------------

    /// Allocates aligned memory from the linear buffer.
    /// @param size Size in bytes to allocate.
    /// @param alignment Alignment requirement (default 256 for CBVs).
    /// @return Allocation containing CPU ptr, GPU VA, and metadata.
    /// @throws std::bad_alloc if capacity is exceeded.
    [[nodiscard]] LinearAllocation Allocate(uint64_t size, uint64_t alignment = 256)
    {
        assert(m_Initialized && "LinearAllocator not initialized");
        assert(size > 0 && "Cannot allocate zero bytes");
        assert((alignment & (alignment - 1)) == 0 && "Alignment must be power of 2");

        // Round up size to alignment boundary
        const uint64_t alignedSize = AlignUp(size, alignment);
        
        // Atomic allocation (lock-free, thread-safe)
        // We use a simple fetch_add; alignment is handled by rounding size
        uint64_t currentOffset;
        uint64_t alignedOffset;
        uint64_t newOffset;

        do
        {
            currentOffset = m_Offset.load(std::memory_order_acquire);
            alignedOffset = AlignUp(currentOffset, alignment);
            newOffset = alignedOffset + alignedSize;

            if (newOffset > m_Capacity)
            {
                // Capacity exceeded - this is a critical error in production
                // Proper handling: grow buffer, use fallback allocator, or fail gracefully
                throw std::bad_alloc();
            }
        } while (!m_Offset.compare_exchange_weak(currentOffset, newOffset,
                                                   std::memory_order_acq_rel,
                                                   std::memory_order_relaxed));

        // Track high water mark for capacity tuning
        uint64_t currentHigh = m_HighWaterMark;
        while (newOffset > currentHigh && 
               !m_HighWaterMark.compare_exchange_weak(currentHigh, newOffset,
                                                       std::memory_order_relaxed));

        LinearAllocation alloc;
        alloc.CpuPtr = m_CpuBase + alignedOffset;
        alloc.GpuAddress = m_GpuBase + alignedOffset;
        alloc.Size = alignedSize;
        alloc.Offset = alignedOffset;

        return alloc;
    }

    /// Convenience method: allocate, copy data, return GPU address.
    /// @tparam T Type of data to copy (must be trivially copyable).
    /// @param data Reference to data to copy into GPU memory.
    /// @return GPU virtual address for binding via SetGraphicsRootConstantBufferView.
    template<typename T>
    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS AllocateAndCopy(const T& data)
    {
        static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
        
        LinearAllocation alloc = Allocate(sizeof(T), 256);
        std::memcpy(alloc.CpuPtr, &data, sizeof(T));
        return alloc.GpuAddress;
    }

    //--------------------------------------------------------------------------
    // Diagnostics
    //--------------------------------------------------------------------------

    /// Returns current allocation offset (bytes used this frame).
    [[nodiscard]] uint64_t GetCurrentOffset() const noexcept 
    { 
        return m_Offset.load(std::memory_order_relaxed); 
    }

    /// Returns total capacity in bytes.
    [[nodiscard]] uint64_t GetCapacity() const noexcept { return m_Capacity; }

    /// Returns peak usage across all frames (for capacity tuning).
    [[nodiscard]] uint64_t GetHighWaterMark() const noexcept 
    { 
        return m_HighWaterMark.load(std::memory_order_relaxed); 
    }

    /// Returns percentage of capacity used this frame.
    [[nodiscard]] float GetUsagePercent() const noexcept
    {
        if (m_Capacity == 0) return 0.0f;
        return static_cast<float>(GetCurrentOffset()) / static_cast<float>(m_Capacity) * 100.0f;
    }

    /// Returns true if allocator is initialized and ready.
    [[nodiscard]] bool IsInitialized() const noexcept { return m_Initialized; }

private:
    /// Aligns a value up to the specified alignment.
    [[nodiscard]] static constexpr uint64_t AlignUp(uint64_t value, uint64_t alignment) noexcept
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

private:
    ComPtr<ID3D12Resource>      m_Resource;
    uint8_t*                    m_CpuBase = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS   m_GpuBase = 0;
    uint64_t                    m_Capacity = 0;
    std::atomic<uint64_t>       m_Offset{ 0 };
    std::atomic<uint64_t>       m_HighWaterMark{ 0 };
    bool                        m_Initialized = false;
};
