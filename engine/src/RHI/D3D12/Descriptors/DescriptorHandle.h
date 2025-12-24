#pragma once

// Lightweight, type-aware descriptor identifier with CPU/GPU handles.
// Constructed from a heap type, a descriptor index, and the heap start handles.
class DescriptorHandle
{
public:
    // Constructs a descriptor handle for a given heap type and index.
    // Parameters:
    //   idx: descriptor index within the heap
    //   type: D3D12 heap type (CBV_SRV_UAV, SAMPLER, RTV, DSV)
    //   cpuStartHandle/gpuStartHandle: start handles of the owning heap
    explicit DescriptorHandle(UINT idx,
                              D3D12_DESCRIPTOR_HEAP_TYPE type,
                              D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle,
                              D3D12_GPU_DESCRIPTOR_HANDLE gpuStartHandle);

    // Default constructor creates an invalid handle (index ~0u, handles 0).
    // Useful for containers and default-constructible classes; must be assigned before use.
    DescriptorHandle() = default;

    // Returns the descriptor index within the heap.
    UINT GetIndex() const noexcept { return m_index; }
    // Returns the CPU descriptor handle for binding or heap management.
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPU() const noexcept { return m_cpuHandle; }
    // Returns the GPU descriptor handle for shader-visible heaps.
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPU() const noexcept { return m_gpuHandle; }
    // Returns the device's descriptor increment size for this heap type.
    UINT GetIncrementSize() const;

    void SetIndex(UINT idx) noexcept { m_index = idx; }
    bool IsValid() const noexcept { return (m_index != InvalidIndex) && (m_cpuHandle.ptr != 0); }
    bool IsShaderVisible() const noexcept { return (m_heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (m_heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER); }

    static constexpr UINT InvalidIndex = ~0u;
private:
    UINT m_index = InvalidIndex; // Descriptor index within the heap (invalid by default)
    D3D12_DESCRIPTOR_HEAP_TYPE m_heapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; 
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle = { 0 }; // CPU handle for descriptor
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle = { 0 }; // GPU handle for descriptor (shader-visible only)
};