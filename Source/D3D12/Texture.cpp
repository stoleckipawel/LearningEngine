#include "Texture.h"

void Texture::Initialize(const std::filesystem::path& imagePath, UINT descriptorHandleIndex)
{
	m_descriptorHandleIndex = descriptorHandleIndex;
	//ToDo:Switch to DirectXTex for better format support and mip generation
	ImageLoader::LoadImageFromDisk(imagePath, m_textureData);
    CreateResource();
    UploadToGPU();
    CreateSRV();
}

void Texture::CreateResource()
{
	//Texture Resource Desc
	m_texResourceDesc = {};
	{
		m_texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		m_texResourceDesc.Width = m_textureData.width;
		m_texResourceDesc.Height = m_textureData.height;
		m_texResourceDesc.DepthOrArraySize = 1;
		m_texResourceDesc.MipLevels = 1;//ToDo: Generate Mips
		m_texResourceDesc.Format = m_textureData.dxgiPixelFormat;
		m_texResourceDesc.SampleDesc.Count = 1;
		m_texResourceDesc.SampleDesc.Quality = 0;
		m_texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		m_texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	CD3DX12_HEAP_PROPERTIES heapDefaultProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(GRHI.Device->CreateCommittedResource(
		&heapDefaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&m_texResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_textureResource)), 
		"Texture: Failed To Create Texture Resource");

	UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_textureResource.Get(), 0, 1);//update when adding mips

	CD3DX12_HEAP_PROPERTIES heapUploadProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ThrowIfFailed(GRHI.Device->CreateCommittedResource(
		&heapUploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_uploadResource)),
		"Texture: Failed To Create Upload Buffer");	
}

void Texture::UploadToGPU()
{
	// collect sub resource data
	const D3D12_SUBRESOURCE_DATA subResourceData = D3D12_SUBRESOURCE_DATA{
		.pData = m_textureData.data.data(),
		.RowPitch = (LONG_PTR)m_textureData.stride,
		.SlicePitch = (LONG_PTR)m_textureData.slicePitch
	};

	UpdateSubresources(
		GRHI.GetCommandList().Get(),
		m_textureResource.Get(),
		m_uploadResource.Get(),
		0, 0,
		(UINT)1,
		&subResourceData);

	// Barrier to PIXEL_SHADER_RESOURCE
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_textureResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	GRHI.GetCommandList()->ResourceBarrier(1, &barrier); 
}

void Texture::CreateSRV()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	{
		srvDesc.Format = m_textureData.dxgiPixelFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;//ToDo: Update when adding mips
	}

	GRHI.Device->CreateShaderResourceView(
		m_textureResource.Get(),
		&srvDesc,
		GetCPUHandle());		
}

void Texture::Release()
{
    m_textureResource.Release();
    m_uploadResource.Release();
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUHandle()
{
	return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetGPUHandle(m_descriptorHandleIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle()
{
	return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCPUHandle(m_descriptorHandleIndex);
} 

Texture::~Texture()
{
	Release();
}
