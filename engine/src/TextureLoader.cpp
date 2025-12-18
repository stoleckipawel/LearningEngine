#include "PCH.h"
#include "TextureLoader.h"
#include "AssetPathResolver.h"

const std::vector<TextureLoader::GUID_to_DXGI> TextureLoader::s_lookupTable = {
    { GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },
    { GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM }
};


// Loads an image from disk and populates m_data
TextureLoader::TextureLoader(const std::filesystem::path& fileName)
{
	std::filesystem::path resolvedPath = ResolveAssetPath(fileName, AssetType::Texture);
    if (!std::filesystem::exists(resolvedPath))
    {
		LogMessage("Texture file not found: " + resolvedPath.string(), ELogType::Fatal);
	}

    // Create WIC Imaging Factory
    ComPtr<IWICImagingFactory> wicFactory;
    ThrowIfFailed(
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(wicFactory.ReleaseAndGetAddressOf())),
        "Failed To Create Factory");

    // Create WIC Stream for file
    ComPtr<IWICStream> wicFileStream;
    ThrowIfFailed(
        wicFactory->CreateStream(wicFileStream.ReleaseAndGetAddressOf()),
        "Failed To Create Stream");

    // Initialize stream from resolvedPath
    ThrowIfFailed(
        wicFileStream->InitializeFromFilename(resolvedPath.wstring().c_str(), GENERIC_READ),
        "Failed To Initialize From Name");

    // Create decoder from stream
    ComPtr<IWICBitmapDecoder> wicDecoder;
    ThrowIfFailed(
        wicFactory->CreateDecoderFromStream(wicFileStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, wicDecoder.ReleaseAndGetAddressOf()),
        "Failed To Create Decoder From Stream");

    // Get first frame of image
    ComPtr<IWICBitmapFrameDecode> wicFrame;
    ThrowIfFailed(
        wicDecoder->GetFrame(0, wicFrame.ReleaseAndGetAddressOf()),
        "Failed To Get Frame");

    // Get image dimensions
    ThrowIfFailed(
        wicFrame->GetSize(&m_data.width, &m_data.height),
        "Failed To Get Size");

    // Get WIC pixel format
    ThrowIfFailed(
        wicFrame->GetPixelFormat(&m_data.wicPixelFormat),
        "Failed To Get Pixel Format");

    // Get pixel format metadata
    ComPtr<IWICComponentInfo> wicComponentInfo;
    ThrowIfFailed(
        wicFactory->CreateComponentInfo(m_data.wicPixelFormat, wicComponentInfo.ReleaseAndGetAddressOf()),
        "Failed To Get Component Info");

    ComPtr<IWICPixelFormatInfo> wicPixelFormatInfo;
    ThrowIfFailed(
        wicComponentInfo->QueryInterface(IID_PPV_ARGS(wicPixelFormatInfo.ReleaseAndGetAddressOf())),
        "Failed To Query Interface");

    // Get bits per pixel
    ThrowIfFailed(
        wicPixelFormatInfo->GetBitsPerPixel(&m_data.bitsPerPixel),
        "Failed To Get Bits Per Pixel");

    // Get channel count
    ThrowIfFailed(
        wicPixelFormatInfo->GetChannelCount(&m_data.channelCount),
        "Failed To Get Channel Count");

    // Map WIC pixel format to DXGI format
    auto findIt = std::find_if(
        TextureLoader::s_lookupTable.begin(), TextureLoader::s_lookupTable.end(),
        [&](const TextureLoader::GUID_to_DXGI& entry) {
            return memcmp(&entry.wic, &m_data.wicPixelFormat, sizeof(GUID)) == 0;
        }
    );
    if (findIt == TextureLoader::s_lookupTable.end()) {
        LogMessage("Unsupported pixel format", ELogType::Fatal);
    }
    m_data.dxgiPixelFormat = findIt->dxgiFormat;

    // Calculate stride and slice pitch
    m_data.stride = ((m_data.bitsPerPixel + 7) / 8) * m_data.width;
    m_data.slicePitch = m_data.height * m_data.stride;
    m_data.data.resize(m_data.slicePitch);

    // Define copy rectangle for pixel data
    WICRect copyRect = { 0, 0, static_cast<INT>(m_data.width), static_cast<INT>(m_data.height) };

    // Copy pixel data to output buffer
    ThrowIfFailed(
        wicFrame->CopyPixels(&copyRect, m_data.stride, m_data.slicePitch, reinterpret_cast<BYTE*>(m_data.data.data())),
        "Failed To Copy Pixels");
}
