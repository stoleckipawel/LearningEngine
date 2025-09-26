#pragma once

#include "ImageLoader.h"
#include "RHI.h"


class FTexture
{
public:
	void Load(const std::filesystem::path& imagePath);
	D3D12_RESOURCE_DESC CreateResourceDesc();
	void Create();
	void Release();
	UINT ComputeMipCount(UINT width, UINT height);

	void CreateSRV(ComPointer<ID3D12DescriptorHeap> srvHeap);

	FImageLoader::ImageData textureData;
	ComPointer<ID3D12Resource2> textureResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
};

