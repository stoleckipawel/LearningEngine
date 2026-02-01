// ============================================================================
// D3D12ConstantBuffer.h
// ----------------------------------------------------------------------------
// Template class for managing typed GPU constant buffers.
//
// USAGE:
//   D3D12ConstantBuffer<PerFrameData> frameCB(descriptorHeapManager);
//   frameCB.Update(frameData);
//   cmdList->SetGraphicsRootConstantBufferView(0, frameCB.GetGPUVirtualAddress());
//
// DESIGN:
//   - Creates persistently-mapped upload buffer (256-byte aligned)
//   - Allocates CBV descriptor from heap manager
//   - Supports both root CBV and descriptor table binding
//
// BINDING METHODS:
//   - GetGPUVirtualAddress(): For SetGraphicsRootConstantBufferView (preferred)
//   - GetGPUHandle(): For descriptor table binding
//
// NOTES:
//   - Non-copyable/non-movable (owns GPU resource and descriptor)
//   - Update() writes directly to mapped memory
// ============================================================================

#pragma once

#include "D3D12DescriptorHeap.h"
#include "D3D12DescriptorHeapManager.h"
#include "DebugUtils.h"
#include <cstring>
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class D3D12Rhi;

template <typename T> class D3D12ConstantBuffer
{
  public:
	// Create and map constant buffer, create a CBV view. Allocates a descriptor via the manager.
	explicit D3D12ConstantBuffer(D3D12Rhi& rhi, D3D12DescriptorHeapManager& descriptorHeapManager) :
	    m_rhi(&rhi),
	    m_descriptorHeapManager(&descriptorHeapManager),
	    m_cbvHandle(descriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)),
	    m_ConstantBufferSize((sizeof(T) + 255) & ~255)
	{
		std::memset(&m_ConstantBufferData, 0, sizeof(T));
		CreateResource();
		CreateConstantBufferView();
	}

	// Updates the buffer with new data
	void Update(const T& Data) noexcept
	{
		m_ConstantBufferData = Data;
		if (m_MappedData)
		{
			std::memcpy(m_MappedData, &m_ConstantBufferData, sizeof(T));
		}
	}

	// Returns the GPU virtual address for root CBV binding (SetGraphicsRootConstantBufferView)
	// This is the preferred binding method for frequently-updated constant buffers.
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept { return Resource ? Resource->GetGPUVirtualAddress() : 0; }

	// Returns the GPU descriptor handle for descriptor table binding
	// Use this only when binding via descriptor tables, not for root CBVs.
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_cbvHandle.GetGPU(); }

	// Returns the CPU descriptor handle for descriptor heap management
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_cbvHandle.GetCPU(); }

	// Returns the aligned size of the backing constant buffer in bytes (256-byte aligned)
	UINT GetSizeInBytes() const noexcept { return m_ConstantBufferSize; }

	// Returns true if the buffer resource is valid and mapped
	bool IsValid() const noexcept { return Resource != nullptr && m_MappedData != nullptr; }

  public:
	// No copy or move allowed, strict ownership
	D3D12ConstantBuffer(const D3D12ConstantBuffer&) = delete;
	D3D12ConstantBuffer& operator=(const D3D12ConstantBuffer&) = delete;
	D3D12ConstantBuffer(D3D12ConstantBuffer&&) = delete;
	D3D12ConstantBuffer& operator=(D3D12ConstantBuffer&&) = delete;

	~D3D12ConstantBuffer() noexcept
	{
		if (Resource)
		{
			Resource->Unmap(0, nullptr);
			Resource.Reset();
		}
		m_MappedData = nullptr;

		if (m_cbvHandle.IsValid())
		{
			m_descriptorHeapManager->FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_cbvHandle);
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

		CHECK(m_rhi->GetDevice()->CreateCommittedResource(
		    &heapProperties,
		    D3D12_HEAP_FLAG_NONE,
		    &resourceDesc,
		    D3D12_RESOURCE_STATE_GENERIC_READ,
		    nullptr,
		    IID_PPV_ARGS(&Resource)));

		DebugUtils::SetDebugName(Resource, L"RHI_ConstantBuffer");

		// Map the resource for CPU writes
		D3D12_RANGE readRange = {0, 0};
		void* mapped = nullptr;

		CHECK(Resource->Map(0, &readRange, &mapped));
		m_MappedData = mapped;
	}

	// Creates a constant buffer view at the given CPU descriptor handle
	void CreateConstantBufferView()
	{
		m_ConstantBufferViewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
		m_ConstantBufferViewDesc.SizeInBytes = m_ConstantBufferSize;
		m_rhi->GetDevice()->CreateConstantBufferView(&m_ConstantBufferViewDesc, GetCPUHandle());
	}

  private:
	D3D12Rhi* m_rhi = nullptr;                                      // RHI reference
	D3D12DescriptorHeapManager* m_descriptorHeapManager = nullptr;  // Descriptor heap manager reference
	ComPtr<ID3D12Resource2> Resource = nullptr;
	D3D12DescriptorHandle m_cbvHandle;  // CBV descriptor handle
	T m_ConstantBufferData;             // Cached buffer data
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantBufferViewDesc = {};
	void* m_MappedData = nullptr;  // Pointer to mapped memory
	UINT m_ConstantBufferSize;     // Aligned buffer size (256 bytes)
};
