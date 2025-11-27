#pragma once

/**
 * @brief ImageLoader provides static methods for loading images from disk using WIC.
 */
class ImageLoader
{
public:
	/**
	 * @brief Structure holding loaded image data and metadata.
	 */
	struct ImageData
	{
		std::vector<char> data;         // Raw image pixel data
		uint32_t width = 1;             // Image width in pixels
		uint32_t height = 1;            // Image height in pixels
		uint32_t bitsPerPixel = 1;      // Bits per pixel
		uint32_t channelCount = 1;      // Number of color channels
		uint32_t stride = 1;            // Row pitch in bytes
		uint32_t slicePitch = 1;        // Total image size in bytes

		GUID wicPixelFormat = {};       // WIC pixel format GUID
		DXGI_FORMAT dxgiPixelFormat = DXGI_FORMAT_UNKNOWN;
	};

	/**
	 * @brief Loads an image from disk and populates ImageData.
	 * @param imagePath Path to the image file.
	 * @param data Output structure for image data and metadata.
	 * @return True if successful, false otherwise.
	 */
	static bool LoadImageFromDisk(const std::filesystem::path& imagePath, ImageData& data);

private:
	/**
	 * @brief Helper structure for mapping WIC pixel formats to DXGI formats.
	 */
	struct GUID_to_DXGI
	{
		GUID wic;
		DXGI_FORMAT dxgiFormat;
	};

	static const std::vector<GUID_to_DXGI> s_lookupTable; // Lookup table for pixel format conversion

	ImageLoader() = default;
	ImageLoader(const ImageLoader&) = default;
	ImageLoader& operator=(const ImageLoader&) = default;
};

