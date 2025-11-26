#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "DescriptorHeap.h"
#include "UploadBuffer.h"
#include "DescriptorHeapManager.h"


// ConstantBuffer manages a GPU constant buffer for type T, including creation, mapping, updating, and descriptor views.
template <typename T>
class ConstantBuffer
{
public:
    // Constructor: acquires and maps the constant buffer resource
    explicit ConstantBuffer(UINT DescriptorHandleIndex)
        : m_DescriptorHandleIndex(DescriptorHandleIndex),
          m_ConstantBufferData{},
          m_ConstantBufferViewDesc{},
          m_MappedData(nullptr),
          m_ConstantBufferSize((sizeof(T) + 255) & ~255),
          Resource(nullptr)
    {
        // Zero-initialize buffer data
        ZeroMemory(&m_ConstantBufferData, sizeof(T));

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

        // Create the committed resource
        ThrowIfFailed(GRHI.Device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&Resource)),
            "Failed to create constant buffer resource.");

        // Map the resource for CPU writes
        D3D12_RANGE readRange = { 0, 0 };
        ThrowIfFailed(Resource->Map(0, &readRange, &m_MappedData), "Failed to map constant buffer resource.");

    }

    // Deleted copy constructor and assignment for strict ownership
    ConstantBuffer(const ConstantBuffer&) = delete;
    ConstantBuffer& operator=(const ConstantBuffer&) = delete;

    // Move constructor
    ConstantBuffer(ConstantBuffer&& other) noexcept
        : m_DescriptorHandleIndex(other.m_DescriptorHandleIndex),
          m_ConstantBufferData(std::move(other.m_ConstantBufferData)),
          m_ConstantBufferViewDesc(other.m_ConstantBufferViewDesc),
          m_MappedData(other.m_MappedData),
          m_ConstantBufferSize(other.m_ConstantBufferSize),
          Resource(std::move(other.Resource))
    {
        other.m_MappedData = nullptr;
    }

    // Move assignment
    ConstantBuffer& operator=(ConstantBuffer&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            m_DescriptorHandleIndex = other.m_DescriptorHandleIndex;
            m_ConstantBufferData = std::move(other.m_ConstantBufferData);
            m_ConstantBufferViewDesc = other.m_ConstantBufferViewDesc;
            m_MappedData = other.m_MappedData;
            m_ConstantBufferSize = other.m_ConstantBufferSize;
            Resource = std::move(other.Resource);
            other.m_MappedData = nullptr;
        }
        return *this;
    }

    // Destructor: releases the buffer resource
    ~ConstantBuffer() noexcept
    {
        Release();
    }

    // Updates the buffer with new data
    void Update(const T& Data)
    {
        m_ConstantBufferData = Data;
        memcpy(m_MappedData, &m_ConstantBufferData, sizeof(T));
    }

    // Creates a constant buffer view at the given CPU descriptor handle
    void CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
    {
        m_ConstantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
        m_ConstantBufferViewDesc.SizeInBytes = static_cast<UINT>(m_ConstantBufferSize);
        GRHI.Device->CreateConstantBufferView(&m_ConstantBufferViewDesc, Handle);
    }

    // Returns the GPU descriptor handle for shader access
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept
    {
        return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVGPUHandle(m_DescriptorHandleIndex);
    }

    // Returns the CPU descriptor handle for descriptor heap management
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept
    {
        return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVCPUHandle(m_DescriptorHandleIndex);
    }

    // Returns the descriptor heap index
    UINT GetDescriptorHandleIndex() const noexcept { return m_DescriptorHandleIndex; }

    // Resource handle
    ComPointer<ID3D12Resource2> Resource = nullptr;

private:
    void Release() noexcept
    {
        if (Resource)
        {
            Resource->Unmap(0, nullptr);
            Resource.Release();
            m_MappedData = nullptr;
        }
    }

    UINT m_DescriptorHandleIndex = 0;           // Index in descriptor heap
    T m_ConstantBufferData;                     // Cached buffer data
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantBufferViewDesc = {}; // CBV description
    void* m_MappedData = nullptr;               // Pointer to mapped memory
    UINT m_ConstantBufferSize = 0;              // Aligned buffer size (256 bytes)
};

