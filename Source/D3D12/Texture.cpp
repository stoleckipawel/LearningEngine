#include "Texture.h"

void Texture::Initialize(const std::filesystem::path& imagePath, ID3D12GraphicsCommandList* cmdList, UINT descriptorHandleIndex)
{
	m_DescriptorHandleIndex = descriptorHandleIndex;
	//ToDo:Switch to DirectXTex for better format support and mip generation
	ImageLoader::LoadImageFromDisk("Assets/Textures/ColorCheckerBoard.png", m_textureData);
    CreateResource();
    UploadToGPU(cmdList);
    CreateSRV();
}

void Texture::CreateResource()
{
	//Texture Resource Desc
	D3D12_RESOURCE_DESC texResourceDesc = {};
	{
		texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		texResourceDesc.Width = m_textureData.width;
		texResourceDesc.Height = m_textureData.height;
		texResourceDesc.DepthOrArraySize = 1;
		texResourceDesc.MipLevels = 1;//ToDo: Generate Mips
		texResourceDesc.Format = m_textureData.dxgiPixelFormat;
		texResourceDesc.SampleDesc.Count = 1;
		texResourceDesc.SampleDesc.Quality = 0;
		texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	{
		heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDefaultProperties.CreationNodeMask = 0;
		heapDefaultProperties.VisibleNodeMask = 0;
	}

	ThrowIfFailed(GRHI.Device->CreateCommittedResource(
		&heapDefaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_textureResource)), 
		"Texture: Failed To Create Texture Resource");

	UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_textureResource.Get(), 0, 1);//update when adding mips

	D3D12_HEAP_PROPERTIES heapUploadProperties = {};
	{
		heapUploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapUploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapUploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapUploadProperties.CreationNodeMask = 0;
		heapUploadProperties.VisibleNodeMask = 0;
	}

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

void Texture::UploadToGPU(ID3D12GraphicsCommandList* cmdList)
{
    D3D12_SUBRESOURCE_DATA sub = {};
	{
		sub.pData = m_textureData.data.data();
		sub.RowPitch = m_textureData.width * ((m_textureData.bitsPerPixel + 7) / 8);
		sub.SlicePitch = sub.RowPitch * m_textureData.height;	
	}

	UpdateSubresources(cmdList, m_textureResource.Get(), m_uploadResource.Get(), 0, 0, 1, &sub);

    // Barrier to PIXEL_SHADER_RESOURCE
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_textureResource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    cmdList->ResourceBarrier(1, &barrier);	
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
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
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
	return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetGPUHandle(m_DescriptorHandleIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle()
{
	return GDescriptorHeapManager.GetCBVSRVUAVHeap().GetCPUHandle(m_DescriptorHandleIndex);
} 

Texture::~Texture()
{
	Release();
}
