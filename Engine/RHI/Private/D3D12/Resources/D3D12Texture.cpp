#include "PCH.h"
#include "D3D12Texture.h"
#include "D3D12Rhi.h"
#include "D3D12DescriptorHeapManager.h"
#include "DebugUtils.h"
#include "Log.h"

// Loads the texture from disk and creates all required GPU resources.
// Allocates an SRV descriptor from the CBV/SRV/UAV heap.
D3D12Texture::D3D12Texture(
    const AssetSystem& assetSystem,
    D3D12Rhi& rhi,
    const std::filesystem::path& fileName,
    D3D12DescriptorHeapManager& descriptorHeapManager) :
    m_rhi(rhi),
    m_loader(std::make_unique<TextureLoader>(assetSystem, fileName)),
    m_srvHandle(descriptorHeapManager.AllocateHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)),
    m_descriptorHeapManager(&descriptorHeapManager)
{
	// TODO: Switch to DirectXTex for better format support and mipmap generation.
	// Basic validation: ensure SRV descriptor allocated and loader produced data.
	if (!m_srvHandle.IsValid())
	{
		LOG_FATAL("D3D12Texture: failed to allocate SRV descriptor.");
	}

	if (!m_loader)
	{
		LOG_FATAL("D3D12Texture: loader failed to initialize.");
	}

	CreateResource();
	UploadToGPU();
	CreateShaderResourceView();
}

void D3D12Texture::CreateResource()
{
	// Describe the texture resource
	m_texResourceDesc = {};
	m_texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	const auto& img = m_loader->GetData();
	m_texResourceDesc.Width = static_cast<UINT64>(img.width);
	m_texResourceDesc.Height = static_cast<UINT>(img.height);
	m_texResourceDesc.DepthOrArraySize = 1;
	m_texResourceDesc.MipLevels = 1;  // TODO: Generate mipmaps
	m_texResourceDesc.Format = img.dxgiPixelFormat;
	m_texResourceDesc.SampleDesc.Count = 1;
	m_texResourceDesc.SampleDesc.Quality = 0;
	m_texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Create the default heap resource for the texture
	CD3DX12_HEAP_PROPERTIES heapDefaultProperties(D3D12_HEAP_TYPE_DEFAULT);
	CHECK(m_rhi.GetDevice()->CreateCommittedResource(
	    &heapDefaultProperties,
	    D3D12_HEAP_FLAG_NONE,
	    &m_texResourceDesc,
	    D3D12_RESOURCE_STATE_COPY_DEST,
	    nullptr,
	    IID_PPV_ARGS(m_textureResource.ReleaseAndGetAddressOf())));
	DebugUtils::SetDebugName(m_textureResource, L"RHI_D3D12Texture");

	// Calculate required size for the upload buffer
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_textureResource.Get(), 0, 1);  // Update when adding mipmaps

	// Create the upload heap resource for staging texture data
	CD3DX12_HEAP_PROPERTIES heapUploadProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	CHECK(m_rhi.GetDevice()->CreateCommittedResource(
	    &heapUploadProperties,
	    D3D12_HEAP_FLAG_NONE,
	    &resourceDesc,
	    D3D12_RESOURCE_STATE_GENERIC_READ,
	    nullptr,
	    IID_PPV_ARGS(m_uploadResource.ReleaseAndGetAddressOf())));
}

void D3D12Texture::UploadToGPU()
{
	// Prepare subresource data for upload
	const auto& img = m_loader->GetData();
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = img.data.empty() ? nullptr : img.data.data();
	subResourceData.RowPitch = static_cast<LONG_PTR>(img.stride);
	subResourceData.SlicePitch = static_cast<LONG_PTR>(img.slicePitch);

	// Upload the data to the GPU texture resource
	UpdateSubresources(m_rhi.GetCommandList().Get(), m_textureResource.Get(), m_uploadResource.Get(), 0, 0, (UINT) 1, &subResourceData);

	// Transition the texture resource to PIXEL_SHADER_RESOURCE state
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	    m_textureResource.Get(),
	    D3D12_RESOURCE_STATE_COPY_DEST,
	    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_rhi.GetCommandList()->ResourceBarrier(1, &barrier);
}

void D3D12Texture::CreateShaderResourceView()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = m_loader->GetData().dxgiPixelFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;  // TODO: Update when adding mipmaps

	m_rhi.GetDevice()->CreateShaderResourceView(m_textureResource.Get(), &srvDesc, GetCPUHandle());
}

D3D12Texture::~D3D12Texture() noexcept
{
	m_textureResource.Reset();
	m_uploadResource.Reset();
	// Return SRV descriptor to allocator
	if (m_srvHandle.IsValid())
	{
		m_descriptorHeapManager->FreeHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_srvHandle);
		m_srvHandle = D3D12DescriptorHandle();
	}
}
