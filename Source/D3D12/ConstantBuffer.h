#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "DescriptorHeap.h"
#include "UploadBuffer.h"

template <typename T>
class FConstantBuffer
{
public:
	void Create();
	void Update(const T& data);
	void Release();
	void CreateConstantBufferView(FDescriptorHeap& descriptorHeap);
public:
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
	ComPointer<ID3D12Resource2> Resource = nullptr;
	T constantBufferData;
	UINT constantBufferSize = 0;
};

template <typename T>
void FConstantBuffer<T>::Create()
{
    //Initialize constant Buffer Data members
    ZeroMemory(&constantBufferData, sizeof(T));

    // Calculate the aligned size (256-byte alignment)
    constantBufferSize = (sizeof(T) + 255) & ~255;

    // Describe the constant buffer resource
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // Use UPLOAD heap for CPU access
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = constantBufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create the resource
    HRESULT hr = GRHI.Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, // State for upload heap
        nullptr,
        IID_PPV_ARGS(&Resource)
    );
    ThrowIfFailed(hr, "Failed to create constant buffer resource.");

    // Map the resource and copy the initial data
    void* mappedData = nullptr;
    D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read from this resource
    hr = Resource->Map(0, &readRange, &mappedData);
    ThrowIfFailed(hr, "Failed to map constant buffer resource.");

    memcpy(mappedData, &constantBufferData, constantBufferSize);
    Resource->Unmap(0, nullptr);

}

template <typename T>
void FConstantBuffer<T>::Update(const T& data)
{
    constantBufferData = data;
    // Map the constant buffer resource
    void* mappedData = nullptr;
    D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read from this resource on the CPU
    HRESULT hr = Resource->Map(0, &readRange, &mappedData);
    if (SUCCEEDED(hr))
    {
        // Copy the updated data to the mapped memory
        memcpy(mappedData, &constantBufferData, sizeof(T));
        Resource->Unmap(0, nullptr);
    }
    else
    {
        LogError("Failed to map constant buffer resource.", ELogType::Warning);
    }
}

template <typename T>
void FConstantBuffer<T>::CreateConstantBufferView(FDescriptorHeap& descriptorHeap)
{
    constantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
    constantBufferViewDesc.SizeInBytes = static_cast<UINT>(constantBufferSize);
    GRHI.Device->CreateConstantBufferView(&constantBufferViewDesc, descriptorHeap.heap->GetCPUDescriptorHandleForHeapStart());
}

template <typename T>
void FConstantBuffer<T>::Release()
{

}

