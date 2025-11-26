
#include "ImageLoader.h"

// Lookup table mapping WIC pixel formats to DXGI formats
const std::vector<ImageLoader::GUID_to_DXGI> ImageLoader::s_lookupTable = {
	{ GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },
	{ GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM }
};

/**
 * @brief Loads an image from disk using WIC and populates ImageData.
 * @param imagePath Path to the image file.
 * @param data Output structure for image data and metadata.
 * @return True if successful, false otherwise.
 */
bool ImageLoader::LoadImageFromDisk(const std::filesystem::path& imagePath, ImageData& data)
{
	// Create WIC Imaging Factory
	ComPointer<IWICImagingFactory> wicFactory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory))))
	{
		LogError("ImageLoader: Failed To Create Factory", ELogType::Warning);
		return false;
	}

	// Create WIC Stream for file
	ComPointer<IWICStream> wicFileStream;
	if (FAILED(wicFactory->CreateStream(&wicFileStream)))
	{
		LogError("ImageLoader: Failed To Create Stream", ELogType::Warning);
		return false;
	}

	// Initialize stream from filename
	if (FAILED(wicFileStream->InitializeFromFilename(imagePath.wstring().c_str(), GENERIC_READ)))
	{
		LogError("ImageLoader: Failed To Initialize From Name", ELogType::Warning);
		return false;
	}

	// Create decoder from stream
	ComPointer<IWICBitmapDecoder> wicDecoder;
	if (FAILED(wicFactory->CreateDecoderFromStream(wicFileStream, nullptr, WICDecodeMetadataCacheOnDemand, &wicDecoder)))
	{
		LogError("ImageLoader: Failed To Create Decoder From Stream", ELogType::Warning);
		return false;
	}

	// Get first frame of image
	ComPointer<IWICBitmapFrameDecode> wicFrame;
	if (FAILED(wicDecoder->GetFrame(0, &wicFrame)))
	{
		LogError("ImageLoader: Failed To Get Frame", ELogType::Warning);
		return false;
	}

	// Get image dimensions
	if (FAILED(wicFrame->GetSize(&data.width, &data.height)))
	{
		LogError("ImageLoader: Failed To Get Size", ELogType::Warning);
		return false;
	}

	// Get WIC pixel format
	if (FAILED(wicFrame->GetPixelFormat(&data.wicPixelFormat)))
	{
		LogError("ImageLoader: Failed To Get Pixel Format", ELogType::Warning);
		return false;
	}

	// Get pixel format metadata
	ComPointer<IWICComponentInfo> wicComponentInfo;
	if (FAILED(wicFactory->CreateComponentInfo(data.wicPixelFormat, &wicComponentInfo)))
	{
		LogError("ImageLoader: Failed To Get Component Info", ELogType::Warning);
		return false;
	}

	ComPointer<IWICPixelFormatInfo> wicPixelFormatInfo;
	if (FAILED(wicComponentInfo->QueryInterface(IID_PPV_ARGS(&wicPixelFormatInfo))))
	{
		LogError("ImageLoader: Failed To Query Interface", ELogType::Warning);
		return false;
	}

	// Get bits per pixel
	if (FAILED(wicPixelFormatInfo->GetBitsPerPixel(&data.bitsPerPixel)))
	{
		LogError("ImageLoader: Failed To Get Bits Per Pixel", ELogType::Warning);
		return false;
	}

	// Get channel count
	if (FAILED(wicPixelFormatInfo->GetChannelCount(&data.channelCount)))
	{
		LogError("ImageLoader: Failed To Get Channel Count", ELogType::Warning);
		return false;
	}

	// Map WIC pixel format to DXGI format
	auto findIt = std::find_if(s_lookupTable.begin(), s_lookupTable.end(),
		[&](const GUID_to_DXGI& entry) {
			return memcmp(&entry.wic, &data.wicPixelFormat, sizeof(GUID)) == 0;
		}
	);
	if (findIt == s_lookupTable.end())
	{
		LogError("ImageLoader: Unsupported pixel format", ELogType::Warning);
		return false;
	}
	data.dxgiPixelFormat = findIt->dxgiFormat;

	// Calculate stride and slice pitch
	data.stride = ((data.bitsPerPixel + 7) / 8) * data.width;
	data.slicePitch = data.height * data.stride;
	data.data.resize(data.slicePitch);

	// Define copy rectangle for pixel data
	WICRect copyRect = { 0, 0, static_cast<INT>(data.width), static_cast<INT>(data.height) };

	// Copy pixel data to output buffer
	ThrowIfFailed(
		wicFrame->CopyPixels(&copyRect, data.stride, data.slicePitch, reinterpret_cast<BYTE*>(data.data.data())),
		"ImageLoader: Failed To Copy Pixels"
	);

	return true;
}
