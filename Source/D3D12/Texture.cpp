
#include "Texture.h"

// Loads the texture from disk and creates all required GPU resources
Texture::Texture(const std::filesystem::path& imagePath, UINT descriptorHandleIndex)
	: m_descriptorHandleIndex(descriptorHandleIndex)
{
	// TODO: Switch to DirectXTex for better format support and mipmap generation
	ImageLoader::LoadImageFromDisk(imagePath, m_textureData);
	CreateResource();
	UploadToGPU();
	CreateSRV();
}

// Creates the committed GPU resource for the texture and its upload buffer
void Texture::CreateResource()
{
	// Describe the texture resource
	m_texResourceDesc = {};
	m_texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_texResourceDesc.Width = m_textureData.width;
	m_texResourceDesc.Height = m_textureData.height;
	m_texResourceDesc.DepthOrArraySize = 1;
	m_texResourceDesc.MipLevels = 1; // TODO: Generate mipmaps
	m_texResourceDesc.Format = m_textureData.dxgiPixelFormat;
	m_texResourceDesc.SampleDesc.Count = 1;
	m_texResourceDesc.SampleDesc.Quality = 0;
	m_texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Create the default heap resource for the texture
	CD3DX12_HEAP_PROPERTIES heapDefaultProperties(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(
		GRHI.Device->CreateCommittedResource(
			&heapDefaultProperties,
			D3D12_HEAP_FLAG_NONE,
			&m_texResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_textureResource)),
		"Texture: Failed To Create Texture Resource"
	);

	// Calculate required size for the upload buffer
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_textureResource.Get(), 0, 1); // Update when adding mipmaps

	// Create the upload heap resource for staging texture data
	CD3DX12_HEAP_PROPERTIES heapUploadProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	ThrowIfFailed(
		GRHI.Device->CreateCommittedResource(
			&heapUploadProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_uploadResource)),
		"Texture: Failed To Create Upload Buffer"
	);
}

// Uploads the texture data from CPU to GPU resource
void Texture::UploadToGPU()
{
	// Prepare subresource data for upload
	const D3D12_SUBRESOURCE_DATA subResourceData = {
		.pData = m_textureData.data.data(),
		.RowPitch = (LONG_PTR)m_textureData.stride,
		.SlicePitch = (LONG_PTR)m_textureData.slicePitch
	};

	// Upload the data to the GPU texture resource
	UpdateSubresources(
		GRHI.GetCommandList().Get(),
		m_textureResource.Get(),
		m_uploadResource.Get(),
		0, 0,
		(UINT)1,
		&subResourceData
	);

	// Transition the texture resource to PIXEL_SHADER_RESOURCE state
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_textureResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	GRHI.GetCommandList()->ResourceBarrier(1, &barrier);
}

// Creates the shader resource view (SRV) for the texture
void Texture::CreateSRV()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = m_textureData.dxgiPixelFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1; // TODO: Update when adding mipmaps

	GRHI.Device->CreateShaderResourceView(
		m_textureResource.Get(),
		&srvDesc,
		GetCPUHandle()
	);
}

// Returns the GPU descriptor handle for shader access
D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUHandle() const
{
	return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetGPUHandle(m_descriptorHandleIndex, DescriptorType::SRV);
}

// Returns the CPU descriptor handle for descriptor heap management
D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle() const
{
	return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCPUHandle(m_descriptorHandleIndex, DescriptorType::SRV);
}

// Releases all GPU resources associated with the texture
void Texture::Release()
{
	m_textureResource.Release();
	m_uploadResource.Release();
}

// Destructor releases resources
Texture::~Texture()
{
	Release();
}
