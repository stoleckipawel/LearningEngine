#include "D3D12Texture.h"

/*
//Texture
D3D12ImageLoader::ImageData textureData;
D3D12ImageLoader::LoadImageFromDisk("Assets/Textures/ColorCheckerBoard.png", textureData);

uint32_t textureStride = textureData.width * ((textureData.bitsPerPixel + 7) / 8);
uint32_t textureSize = textureStride * textureData.height;
// === Texture ===
D3D12_RESOURCE_DESC textureResourceDesc = D3D12Renderer::Get().CreateTextureResourceDesc(textureData);

ComPointer<ID3D12Resource2> textureResource;
D3D12Context::Get().GetDevice()->CreateCommittedResource(
	&heapDefaultProperties,
	D3D12_HEAP_FLAG_NONE,
	&textureResourceDesc,
	D3D12_RESOURCE_STATE_COMMON,
	nullptr,
	IID_PPV_ARGS(&textureResource));

//SRV
D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
srvDesc.Format = textureData.dxgiPixelFormat;
srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
srvDesc.Texture2D.MostDetailedMip = 0;
srvDesc.Texture2D.MipLevels = 1;
srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
srvDesc.Texture2D.PlaneSlice = 0;
D3D12Context::Get().GetDevice()->CreateShaderResourceView(
	textureResource,
	&srvDesc,
	descHeap->GetCPUDescriptorHandleForHeapStart());
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

//Descriptor Heap for Textures
D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
descHeapDesc.NumDescriptors = 8;
descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
descHeapDesc.NodeMask = 0;

ComPointer<ID3D12DescriptorHeap> descHeap;
D3D12Context::Get().GetDevice()->CreateDescriptorHeap(
	&descHeapDesc,
	IID_PPV_ARGS(&descHeap));Copyright(c) 2003 - 2019 Jason Perkins and individual contributors.
*/