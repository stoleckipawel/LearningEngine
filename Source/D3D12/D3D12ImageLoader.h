#pragma once

#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include <filesystem>

class D3D12ImageLoader
{
public:
	struct ImageData
	{
		std::vector<char> data;
		uint32_t width;
		uint32_t height;
		uint32_t bitsPerPixel;
		uint32_t channelCount;

		GUID wicPixelFormat;
		DXGI_FORMAT dxgiPixelFormat;
	};

	static bool LoadImageFromDisk(const std::filesystem::path& imagePath, ImageData& data);
private:
	struct GUID_to_DXGI
	{
		GUID wic;
		DXGI_FORMAT dxgiFormat;
	};

	static const std::vector<GUID_to_DXGI> s_lookupTable;

	D3D12ImageLoader() = default;
	D3D12ImageLoader(const D3D12ImageLoader&) = default;
	D3D12ImageLoader& operator=(const D3D12ImageLoader&) = default;
};

