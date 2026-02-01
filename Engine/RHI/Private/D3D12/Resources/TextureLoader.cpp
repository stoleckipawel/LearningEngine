#include "PCH.h"
#include "TextureLoader.h"
#include "Assets/AssetSystem.h"
#include "Log.h"
#include <cstring>
#include <limits>

const std::vector<TextureLoader::GUID_to_DXGI> TextureLoader::s_lookupTable = {
    {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM},
    {GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM}};

TextureLoader::TextureLoader(const AssetSystem& assetSystem, const std::filesystem::path& fileName)
{
	const auto resolvedPath = assetSystem.ResolvePathValidated(fileName, AssetType::Texture);

	ComPtr<IWICImagingFactory> wicFactory;
	CHECK(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(wicFactory.ReleaseAndGetAddressOf())));

	ComPtr<IWICBitmapFrameDecode> wicFrame = DecodeImageFile(wicFactory.Get(), resolvedPath);
	QueryPixelFormat(wicFactory.Get(), wicFrame.Get());
	MapToDxgiFormat(resolvedPath);
	CalculateBufferLayout();
	CopyPixelData(wicFrame.Get());
}

// =============================================================================
// Loading Helpers
// =============================================================================

ComPtr<IWICBitmapFrameDecode> TextureLoader::DecodeImageFile(IWICImagingFactory* wicFactory, const std::filesystem::path& resolvedPath)
{
	ComPtr<IWICStream> wicFileStream;
	CHECK(wicFactory->CreateStream(wicFileStream.ReleaseAndGetAddressOf()));
	CHECK(wicFileStream->InitializeFromFilename(resolvedPath.wstring().c_str(), GENERIC_READ));

	ComPtr<IWICBitmapDecoder> wicDecoder;
	CHECK(wicFactory
	          ->CreateDecoderFromStream(wicFileStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, wicDecoder.ReleaseAndGetAddressOf()));

	ComPtr<IWICBitmapFrameDecode> wicFrame;
	CHECK(wicDecoder->GetFrame(0, wicFrame.ReleaseAndGetAddressOf()));

	CHECK(wicFrame->GetSize(&m_data.width, &m_data.height));

	return wicFrame;
}

void TextureLoader::QueryPixelFormat(IWICImagingFactory* wicFactory, IWICBitmapFrameDecode* wicFrame)
{
	CHECK(wicFrame->GetPixelFormat(&m_data.wicPixelFormat));

	ComPtr<IWICComponentInfo> wicComponentInfo;
	CHECK(wicFactory->CreateComponentInfo(m_data.wicPixelFormat, wicComponentInfo.ReleaseAndGetAddressOf()));

	ComPtr<IWICPixelFormatInfo> wicPixelFormatInfo;
	CHECK(wicComponentInfo->QueryInterface(IID_PPV_ARGS(wicPixelFormatInfo.ReleaseAndGetAddressOf())));

	CHECK(wicPixelFormatInfo->GetBitsPerPixel(&m_data.bitsPerPixel));
	CHECK(wicPixelFormatInfo->GetChannelCount(&m_data.channelCount));
}

void TextureLoader::MapToDxgiFormat(const std::filesystem::path& resolvedPath)
{
	auto findIt = std::find_if(
	    s_lookupTable.begin(),
	    s_lookupTable.end(),
	    [&](const GUID_to_DXGI& entry)
	    {
		    return std::memcmp(&entry.wic, &m_data.wicPixelFormat, sizeof(GUID)) == 0;
	    });

	if (findIt == s_lookupTable.end())
	{
		LOG_FATAL(std::string("Unsupported pixel format for file: ") + resolvedPath.string());
	}

	m_data.dxgiPixelFormat = findIt->dxgiFormat;
}

void TextureLoader::CalculateBufferLayout()
{
	// Use 64-bit arithmetic to avoid overflow on large textures
	const uint64_t bytesPerPixel = static_cast<uint64_t>((m_data.bitsPerPixel + 7) / 8);
	const uint64_t stride64 = bytesPerPixel * static_cast<uint64_t>(m_data.width);
	const uint64_t slicePitch64 = stride64 * static_cast<uint64_t>(m_data.height);

	if (stride64 > (std::numeric_limits<uint32_t>::max)() || slicePitch64 > (std::numeric_limits<size_t>::max)())
	{
		LOG_FATAL("Texture too large or stride overflow");
	}

	m_data.stride = static_cast<uint32_t>(stride64);
	m_data.slicePitch = static_cast<uint32_t>(slicePitch64);
	m_data.data.resize(static_cast<size_t>(m_data.slicePitch));
}

void TextureLoader::CopyPixelData(IWICBitmapFrameDecode* wicFrame)
{
	WICRect copyRect = {0, 0, static_cast<INT>(m_data.width), static_cast<INT>(m_data.height)};
	CHECK(wicFrame->CopyPixels(&copyRect, m_data.stride, m_data.slicePitch, reinterpret_cast<BYTE*>(m_data.data.data())));
}
