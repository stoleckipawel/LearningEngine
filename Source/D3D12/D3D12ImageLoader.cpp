#include "D3D12ImageLoader.h"
#include <algorithm>

const std::vector<D3D12ImageLoader::GUID_to_DXGI> D3D12ImageLoader::s_lookupTable =
{
	{ GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },
	{ GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM }
};

bool D3D12ImageLoader::LoadImageFromDisk(const std::filesystem::path& imagePath, ImageData& data)
{
	// Create Factory
	ComPointer<IWICImagingFactory> wicFactory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory))))
	{
		return false;
	}

	// Load Image
	ComPointer<IWICStream> wicFileStream;
	if (FAILED(wicFactory->CreateStream(&wicFileStream)))
	{
		return false;
	}

	if (FAILED(wicFileStream->InitializeFromFilename(imagePath.wstring().c_str(), GENERIC_READ)))
	{
		return false;
	}

	ComPointer<IWICBitmapDecoder> wicDecoder;
	if (FAILED(wicFactory->CreateDecoderFromStream(wicFileStream, nullptr, WICDecodeMetadataCacheOnDemand, &wicDecoder)))
	{
		return false;
	}

	// Get Frame
	ComPointer<IWICBitmapFrameDecode> wicFrame;
	if (FAILED(wicDecoder->GetFrame(0, &wicFrame)))
	{
		return false;
	}

	// Image MetaData
	if (FAILED(wicFrame->GetSize(&data.width, &data.height)))
	{
		return false;
	}

	if (FAILED(wicFrame->GetPixelFormat(&data.wicPixelFormat)))
	{
		return false;
	}

	//Meta Data of Pixel Format
	ComPointer<IWICComponentInfo> wicComponentInfo;
	if (FAILED(wicFactory->CreateComponentInfo(data.wicPixelFormat, &wicComponentInfo)))
	{
		return false;
	}

	ComPointer<IWICPixelFormatInfo> wicPixelFormatInfo;
	if (FAILED(wicComponentInfo->QueryInterface(IID_PPV_ARGS(&wicPixelFormatInfo))))
	{
		return false;
	}

	// Get Bits Per Pixel
	if (FAILED(wicPixelFormatInfo->GetBitsPerPixel(&data.bitsPerPixel)))
	{
		return false;
	}

	// Get Channel Count
	if (FAILED(wicPixelFormatInfo->GetChannelCount(&data.channelCount)))
	{
		return false;
	}

	// Get DXGI Format
	auto findIt = std::find_if(s_lookupTable.begin(), s_lookupTable.end(),
		[&](const GUID_to_DXGI& entry)
		{
			return memcmp(&entry.wic, &data.wicPixelFormat, sizeof(GUID)) == 0;
		}
	);
	if (findIt == s_lookupTable.end())
	{
		return false;
	}
	data.dxgiPixelFormat = findIt->dxgiFormat;

	//Image Loading
	uint32_t stride = ((data.bitsPerPixel + 7) / 8) * data.width;
	uint32_t size = data.height * stride;
	data.data.resize(size);

	WICRect copyRect;
	copyRect.X = 0;
	copyRect.Y = 0;
	copyRect.Width = data.width;
	copyRect.Height = data.height;
	if (FAILED(wicFrame->CopyPixels(&copyRect, stride, size, reinterpret_cast<BYTE*>(data.data.data()))))
	{
		return false;
	}

	return true;
}
