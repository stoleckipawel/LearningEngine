#include "PCH.h"
#include "D3D12/DescriptorHandle.h"
#include "D3D12/RHI.h"

// Constructs a descriptor handle for a given heap type and index.
// Computes CPU/GPU handles using heap start and device increment size.
DescriptorHandle::DescriptorHandle(UINT idx,
                                   D3D12_DESCRIPTOR_HEAP_TYPE type,
                                   D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle,
                                   D3D12_GPU_DESCRIPTOR_HANDLE gpuStartHandle)
    : m_index(idx), m_heapType(type)
{
    // Compute CPU handle: heap start + index * increment size
    m_cpuHandle.ptr = cpuStartHandle.ptr + GetIncrementSize() * m_index;

    // Compute GPU handle only for shader-visible heaps
    if (IsShaderVisible())
    {
        m_gpuHandle.ptr = gpuStartHandle.ptr + GetIncrementSize() * m_index;
    }
}

// Returns the device's descriptor increment size for this heap type.
UINT DescriptorHandle::GetIncrementSize() const
{
    return GRHI.GetDevice()->GetDescriptorHandleIncrementSize(m_heapType);
}