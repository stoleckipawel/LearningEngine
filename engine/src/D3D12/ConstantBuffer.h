#pragma once

#include "DescriptorHeap.h"
#include "UploadBuffer.h"
#include "DescriptorHeapManager.h"
#include "DebugUtils.h"

// ConstantBuffer manages a GPU constant buffer for type T, including creation, mapping, updating, and descriptor views.
template <typename T>
class ConstantBuffer
{
public:
    // Create and map constant buffer, create a CBV view. Allocates a descriptor via the manager.
    explicit ConstantBuffer()
        : m_cbvHandle(GDescriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)),
          m_ConstantBufferSize((sizeof(T) + 255) & ~255)
    {
        ZeroMemory(&m_ConstantBufferData, sizeof(T));
        CreateResource();
        CreateConstantBufferView();
    }

    // Updates the buffer with new data
    void Update(const T& Data)
    {
        m_ConstantBufferData = Data;
        memcpy(m_MappedData, &m_ConstantBufferData, sizeof(T));
    }

    // Returns the GPU descriptor handle for shader access
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_cbvHandle.GetGPU(); }

    // Returns the CPU descriptor handle for descriptor heap management
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_cbvHandle.GetCPU(); }
public:
    // No copy or move allowed, strict ownership
    ConstantBuffer(const ConstantBuffer&) = delete;
    ConstantBuffer& operator=(const ConstantBuffer&) = delete;
    ConstantBuffer(ConstantBuffer&&) = delete;
    ConstantBuffer& operator=(ConstantBuffer&&) = delete;

    ~ConstantBuffer() noexcept
    {
        Resource->Unmap(0, nullptr);
        Resource.Reset();
        m_MappedData = nullptr;

        if (m_cbvHandle.IsValid())
        {
            GDescriptorHeapManager.FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_cbvHandle);
        }
    }
private:
    // Create the committed resource and map for CPU writes
    void CreateResource()
    {
        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = m_ConstantBufferSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        ThrowIfFailed(GRHI.GetDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&Resource)),
            "Failed to create constant buffer resource.");
        DebugUtils::SetDebugName(Resource, L"RHI_ConstantBuffer");

        // Map the resource for CPU writes
        D3D12_RANGE readRange = { 0, 0 };
        ThrowIfFailed(Resource->Map(0, &readRange, &m_MappedData), "Failed to map constant buffer resource.");
    }

    // Creates a constant buffer view at the given CPU descriptor handle
    void CreateConstantBufferView()
    {
        m_ConstantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
        m_ConstantBufferViewDesc.SizeInBytes = m_ConstantBufferSize;
        GRHI.GetDevice()->CreateConstantBufferView(&m_ConstantBufferViewDesc, GetCPUHandle());
    }
private:
    ComPtr<ID3D12Resource2> Resource = nullptr;
    DescriptorHandle m_cbvHandle; // CBV descriptor handle
    T m_ConstantBufferData; // Cached buffer data
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantBufferViewDesc = {}; 
    void* m_MappedData = nullptr; // Pointer to mapped memory
    UINT m_ConstantBufferSize; // Aligned buffer size (256 bytes)
};

