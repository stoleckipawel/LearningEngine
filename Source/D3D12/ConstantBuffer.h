#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "DescriptorHeap.h"
#include "UploadBuffer.h"

template <typename T>
class FConstantBuffer
{
public:
	void Initialize(UINT HandleIndex);
	void Update(const T& data);
	void Release();
	void CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE handle);
public:
    D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc = {};
	ComPointer<ID3D12Resource2> Resource = nullptr;
	T ConstantBufferData;
	UINT ConstantBufferSize = 0;
    void* MappedData = nullptr;
    UINT HandleIndex = 0;
};

template <typename T>
void FConstantBuffer<T>::Initialize(UINT HandleIndex)
{
    this->HandleIndex = HandleIndex;

    //Initialize constant Buffer Data members
    ZeroMemory(&ConstantBufferData, sizeof(T));

    // Calculate the aligned size (256-byte alignment)
    ConstantBufferSize = (sizeof(T) + 255) & ~255;

    // Describe the constant buffer resource
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // Use UPLOAD heap for CPU access
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = ConstantBufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Create the resource
    ThrowIfFailed(GRHI.Device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&Resource)),
        "Failed to create constant buffer resource.");

    // Map the resource and copy the initial data
    D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read from this resource
    ThrowIfFailed(Resource->Map(0, &readRange, &MappedData), "Failed to map constant buffer resource.");
}

template <typename T>
void FConstantBuffer<T>::Update(const T& data)
{
  ConstantBufferData = data;

  // Copy the updated data to the mapped memory
  memcpy(MappedData, &ConstantBufferData, sizeof(T));
}

template <typename T>
void FConstantBuffer<T>::CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
    ConstantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
    ConstantBufferViewDesc.SizeInBytes = static_cast<UINT>(ConstantBufferSize);
    GRHI.Device->CreateConstantBufferView(&ConstantBufferViewDesc, handle);
}

template <typename T>
void FConstantBuffer<T>::Release()
{
    Resource->Unmap(0, nullptr);
    Resource.Release();
}

