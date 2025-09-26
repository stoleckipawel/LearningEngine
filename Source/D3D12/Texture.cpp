#include "Texture.h"


void FTexture::Load(const std::filesystem::path& imagePath)
{
	FImageLoader::LoadImageFromDisk("Assets/Textures/ColorCheckerBoard.png", textureData);
}

UINT FTexture::ComputeMipCount(UINT width, UINT height)
{
	return 1;

	//ToDo activate when mip generation is done
	return 1 + static_cast<UINT>(floor(log2(static_cast<float>(std::max(width, height)))));
}

D3D12_RESOURCE_DESC FTexture::CreateResourceDesc()
{
	D3D12_RESOURCE_DESC texResourceDesc = {};
	texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	texResourceDesc.Width = textureData.width;
	texResourceDesc.Height = textureData.height;
	texResourceDesc.DepthOrArraySize = 1;
	texResourceDesc.MipLevels = ComputeMipCount(textureData.width, textureData.height);
	texResourceDesc.Format = textureData.dxgiPixelFormat;
	texResourceDesc.SampleDesc.Count = 1;
	texResourceDesc.SampleDesc.Quality = 0;
	texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	return texResourceDesc;
}

void FTexture::Create()
{
	D3D12_RESOURCE_DESC texResourceDesc = CreateResourceDesc();

	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

	GRHI.Device->CreateCommittedResource(
		&heapDefaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&textureResource));
}

void FTexture::Release()
{
	textureResource.Release();
}

void FTexture::CreateSRV(ComPointer<ID3D12DescriptorHeap> srvHeap)
{
	srvDesc.Format = textureData.dxgiPixelFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = ComputeMipCount(textureData.width, textureData.height);
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;

	GRHI.Device->CreateShaderResourceView(
		textureResource,
		&srvDesc,
		srvHeap->GetCPUDescriptorHandleForHeapStart());
}

/*
	uint32_t textureStride = textureData.width * ((textureData.bitsPerPixel + 7) / 8);
	uint32_t textureSize = textureStride * textureData.height;


D3D12_BOX textureSizeAsBox;
textureSizeAsBox.left = textureSizeAsBox.top = textureSizeAsBox.front = 0;
textureSizeAsBox.right = textureData.width;
textureSizeAsBox.bottom = textureData.height;
textureSizeAsBox.back = 1;

D3D12_TEXTURE_COPY_LOCATION txtcSrc;

txtcSrc.pResource = uploadBuffer;
txtcSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
txtcSrc.PlacedFootprint.Offset = 0;
txtcSrc.PlacedFootprint.Footprint.Width = textureData.width;
txtcSrc.PlacedFootprint.Footprint.Height = textureData.height;
txtcSrc.PlacedFootprint.Footprint.Depth = 1;
txtcSrc.PlacedFootprint.Footprint.RowPitch = textureStride;
txtcSrc.PlacedFootprint.Footprint.Format = textureData.dxgiPixelFormat;

D3D12_TEXTURE_COPY_LOCATION	txtcDst;
txtcDst.pResource = textureResource;
txtcDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
txtcDst.SubresourceIndex = 0;
cmdList->CopyTextureRegion(&txtcDst, 0, 0, 0, &txtcSrc, &textureSizeAsBox);


*/

