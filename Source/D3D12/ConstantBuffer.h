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
    // Initializes the constant buffer resource and maps it for CPU access
    void Initialize(UINT DescriptorHandleIndex);
    // Updates the buffer with new data
    void Update(const T& Data);
    // Releases the buffer resource
    void Release();
    // Creates a constant buffer view at the given CPU descriptor handle
    void CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Handle);
    // Returns the GPU descriptor handle for shader access
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
    // Returns the CPU descriptor handle for descriptor heap management
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
    // Returns the descriptor heap index
    UINT GetDescriptorHandleIndex() const { return m_DescriptorHandleIndex; }

    ComPointer<ID3D12Resource2> Resource = nullptr;
private:
    UINT m_DescriptorHandleIndex = 0;           // Index in descriptor heap
    T m_ConstantBufferData;                     // Cached buffer data
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantBufferViewDesc = {}; // CBV description
    void* m_MappedData = nullptr;               // Pointer to mapped memory
    UINT m_ConstantBufferSize = 0;              // Aligned buffer size (256 bytes)
};


// Initializes the constant buffer resource and maps it for CPU access
template <typename T>
void ConstantBuffer<T>::Initialize(UINT DescriptorHandleIndex)
{
    m_DescriptorHandleIndex = DescriptorHandleIndex;

    // Zero-initialize buffer data
    ZeroMemory(&m_ConstantBufferData, sizeof(T));

    // Calculate aligned size (256 bytes required by D3D12)
    m_ConstantBufferSize = (sizeof(T) + 255) & ~255;

    // Describe the constant buffer resource
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // Use UPLOAD heap for CPU access
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
    D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read from this resource
    ThrowIfFailed(Resource->Map(0, &readRange, &m_MappedData), "Failed to map constant buffer resource.");
}


// Updates the buffer with new data
template <typename T>
void ConstantBuffer<T>::Update(const T& Data)
{
    m_ConstantBufferData = Data;
    // Copy the updated data to the mapped memory
    memcpy(m_MappedData, &m_ConstantBufferData, sizeof(T));
}


// Creates a constant buffer view at the given CPU descriptor handle
template <typename T>
void ConstantBuffer<T>::CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
{
    m_ConstantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
    m_ConstantBufferViewDesc.SizeInBytes = static_cast<UINT>(m_ConstantBufferSize);
    GRHI.Device->CreateConstantBufferView(&m_ConstantBufferViewDesc, Handle);
}


// Releases the buffer resource
template <typename T>
void ConstantBuffer<T>::Release()
{
    if (Resource)
    {
        Resource->Unmap(0, nullptr);
        Resource.Release();
    }
}


// Returns the GPU descriptor handle for shader access
template <typename T>
D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer<T>::GetGPUHandle()
{
    return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVGPUHandle(m_DescriptorHandleIndex);
}   


// Returns the CPU descriptor handle for descriptor heap management
template <typename T> 
D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer<T>::GetCPUHandle()
{
    return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCBVCPUHandle(m_DescriptorHandleIndex);
}
