#pragma once

#include "D3D12ImageLoader.h"


class D3D12Texture
{
public:
	void Load(const std::filesystem::path& imagePath);
	D3D12_RESOURCE_DESC CreateResourceDesc();
	void CreateResource();
	UINT ComputeMipCount(UINT width, UINT height);

	D3D12ImageLoader::ImageData textureData;
};

