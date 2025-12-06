#include "PCH.h"
#include "D3D12/TextureLoader.h"
#include "D3D12/AssetPathResolver.h"

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
    ComPointer<IWICImagingFactory> wicFactory;
    ThrowIfFailed(
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)),
        "TextureLoader: Failed To Create Factory", ELogType::Fatal);

    // Create WIC Stream for file
    ComPointer<IWICStream> wicFileStream;
    ThrowIfFailed(
        wicFactory->CreateStream(&wicFileStream),
        "TextureLoader: Failed To Create Stream", ELogType::Fatal);

    // Initialize stream from resolvedPath
    ThrowIfFailed(
        wicFileStream->InitializeFromFilename(resolvedPath.wstring().c_str(), GENERIC_READ),
        "TextureLoader: Failed To Initialize From Name", ELogType::Fatal);

    // Create decoder from stream
    ComPointer<IWICBitmapDecoder> wicDecoder;
    ThrowIfFailed(
        wicFactory->CreateDecoderFromStream(wicFileStream, nullptr, WICDecodeMetadataCacheOnDemand, &wicDecoder),
        "TextureLoader: Failed To Create Decoder From Stream", ELogType::Fatal);

    // Get first frame of image
    ComPointer<IWICBitmapFrameDecode> wicFrame;
    ThrowIfFailed(
        wicDecoder->GetFrame(0, &wicFrame),
        "TextureLoader: Failed To Get Frame", ELogType::Fatal);

    // Get image dimensions
    ThrowIfFailed(
        wicFrame->GetSize(&m_data.width, &m_data.height),
        "TextureLoader: Failed To Get Size", ELogType::Fatal);

    // Get WIC pixel format
    ThrowIfFailed(
        wicFrame->GetPixelFormat(&m_data.wicPixelFormat),
        "TextureLoader: Failed To Get Pixel Format", ELogType::Fatal);

    // Get pixel format metadata
    ComPointer<IWICComponentInfo> wicComponentInfo;
    ThrowIfFailed(
        wicFactory->CreateComponentInfo(m_data.wicPixelFormat, &wicComponentInfo),
        "TextureLoader: Failed To Get Component Info", ELogType::Fatal);

    ComPointer<IWICPixelFormatInfo> wicPixelFormatInfo;
    ThrowIfFailed(
        wicComponentInfo->QueryInterface(IID_PPV_ARGS(&wicPixelFormatInfo)),
        "TextureLoader: Failed To Query Interface", ELogType::Fatal);

    // Get bits per pixel
    ThrowIfFailed(
        wicPixelFormatInfo->GetBitsPerPixel(&m_data.bitsPerPixel),
        "TextureLoader: Failed To Get Bits Per Pixel", ELogType::Fatal);

    // Get channel count
    ThrowIfFailed(
        wicPixelFormatInfo->GetChannelCount(&m_data.channelCount),
        "TextureLoader: Failed To Get Channel Count", ELogType::Fatal);

    // Map WIC pixel format to DXGI format
    auto findIt = std::find_if(
        TextureLoader::s_lookupTable.begin(), TextureLoader::s_lookupTable.end(),
        [&](const TextureLoader::GUID_to_DXGI& entry) {
            return memcmp(&entry.wic, &m_data.wicPixelFormat, sizeof(GUID)) == 0;
        }
    );
    if (findIt == TextureLoader::s_lookupTable.end()) {
        LogMessage("TextureLoader: Unsupported pixel format", ELogType::Fatal);
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
        "TextureLoader: Failed To Copy Pixels", ELogType::Fatal);
}
TextureLoader::~TextureLoader() = default;