#include "PCH.h"
#include "TextureLoader.h"
#include "AssetPathResolver.h"
#include "Log.h"
#include <cstring>
#include <limits>

const std::vector<TextureLoader::GUID_to_DXGI> TextureLoader::s_lookupTable = {
    { GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },
    { GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM }
};


// Loads an image from disk and populates m_data
TextureLoader::TextureLoader(const std::filesystem::path& fileName)
{
    const std::filesystem::path resolvedPath = ResolveAssetPath(fileName, AssetType::Texture);
    if (!std::filesystem::exists(resolvedPath))
    {
        LOG_FATAL(std::string("Texture file not found: ") + resolvedPath.string());
    }

    // Create WIC Imaging Factory
    ComPtr<IWICImagingFactory> wicFactory;
    CHECK(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(wicFactory.ReleaseAndGetAddressOf())));

    // Create and initialize WIC stream for file
    ComPtr<IWICStream> wicFileStream;
    CHECK(wicFactory->CreateStream(wicFileStream.ReleaseAndGetAddressOf()));
    CHECK(wicFileStream->InitializeFromFilename(resolvedPath.wstring().c_str(), GENERIC_READ));

    // Create decoder and get the first frame
    ComPtr<IWICBitmapDecoder> wicDecoder;
    CHECK(wicFactory->CreateDecoderFromStream(wicFileStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, wicDecoder.ReleaseAndGetAddressOf()));

    ComPtr<IWICBitmapFrameDecode> wicFrame;
    CHECK(wicDecoder->GetFrame(0, wicFrame.ReleaseAndGetAddressOf()));

    // Get image dimensions
    CHECK(wicFrame->GetSize(&m_data.width, &m_data.height));

    // Get WIC pixel format
    CHECK(wicFrame->GetPixelFormat(&m_data.wicPixelFormat));

    // Query pixel format metadata
    ComPtr<IWICComponentInfo> wicComponentInfo;
    CHECK(wicFactory->CreateComponentInfo(m_data.wicPixelFormat, wicComponentInfo.ReleaseAndGetAddressOf()));

    ComPtr<IWICPixelFormatInfo> wicPixelFormatInfo;
    CHECK(wicComponentInfo->QueryInterface(IID_PPV_ARGS(wicPixelFormatInfo.ReleaseAndGetAddressOf())));

    // Bits per pixel and channel count
    CHECK(wicPixelFormatInfo->GetBitsPerPixel(&m_data.bitsPerPixel));
    CHECK(wicPixelFormatInfo->GetChannelCount(&m_data.channelCount));

    // Map WIC pixel format to DXGI format
    auto findIt = std::find_if(TextureLoader::s_lookupTable.begin(), TextureLoader::s_lookupTable.end(),
        [&](const TextureLoader::GUID_to_DXGI& entry) {
            return std::memcmp(&entry.wic, &m_data.wicPixelFormat, sizeof(GUID)) == 0;
        });

    if (findIt == TextureLoader::s_lookupTable.end())
    {
        LOG_FATAL(std::string("Unsupported pixel format for file: ") + resolvedPath.string());
    }
    m_data.dxgiPixelFormat = findIt->dxgiFormat;

    // Calculate stride and slice pitch using 64-bit arithmetic to avoid overflow
    const uint64_t bytesPerPixel = static_cast<uint64_t>((m_data.bitsPerPixel + 7) / 8);
    const uint64_t stride64 = bytesPerPixel * static_cast<uint64_t>(m_data.width);
    const uint64_t slicePitch64 = stride64 * static_cast<uint64_t>(m_data.height);

    // Guard against pathological allocations
    if (stride64 > std::numeric_limits<uint32_t>::max() || slicePitch64 > std::numeric_limits<size_t>::max())
    {
        LOG_FATAL("Texture too large or stride overflow");
    }

    m_data.stride = static_cast<uint32_t>(stride64);
    m_data.slicePitch = static_cast<uint32_t>(slicePitch64);
    m_data.data.clear();
    m_data.data.resize(static_cast<size_t>(m_data.slicePitch));

    // Define copy rectangle for pixel data
    WICRect copyRect = { 0, 0, static_cast<INT>(m_data.width), static_cast<INT>(m_data.height) };

    // Copy pixel data to output buffer
    CHECK(wicFrame->CopyPixels(&copyRect, m_data.stride, m_data.slicePitch, reinterpret_cast<BYTE*>(m_data.data.data())));
}
