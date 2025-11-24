#pragma once

#include <vector>
#include "../Vendor/Windows/WinInclude.h"
#include <filesystem>

class ImageLoader
{
public:
	struct ImageData
	{
		std::vector<char> data;
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t bitsPerPixel = 1;
		uint32_t channelCount = 1;
		uint32_t stride = 1;//Row Pitch
		uint32_t slicePitch = 1;

		GUID wicPixelFormat = {};
		DXGI_FORMAT dxgiPixelFormat = DXGI_FORMAT_UNKNOWN;
	};

	static bool LoadImageFromDisk(const std::filesystem::path& imagePath, ImageData& data);
private:
	struct GUID_to_DXGI
	{
		GUID wic;
		DXGI_FORMAT dxgiFormat;
	};

	static const std::vector<GUID_to_DXGI> s_lookupTable;

	ImageLoader() = default;
	ImageLoader(const ImageLoader&) = default;
	ImageLoader& operator=(const ImageLoader&) = default;
};

