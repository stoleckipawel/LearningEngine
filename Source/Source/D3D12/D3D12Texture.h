#pragma once

#include "D3D12ImageLoader.h"
#include "D3D12Context.h"


class D3D12Texture
{
public:
	void Load(const std::filesystem::path& imagePath);
	D3D12_RESOURCE_DESC CreateResourceDesc();
	void CreateResource();
	UINT ComputeMipCount(UINT width, UINT height);

	void CreateSRV(ComPointer<ID3D12DescriptorHeap> srvHeap);

	D3D12ImageLoader::ImageData textureData;
	ComPointer<ID3D12Resource2> textureResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
};

