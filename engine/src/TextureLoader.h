#pragma once

// TextureLoader provides methods for loading images from disk using WIC.
// Notes:
// - The returned pixel buffer uses `uint8_t` for unambiguous byte-sized storage.
// - The loader only supports a small set of WIC pixel formats (expand as needed).
class TextureLoader
{
public:
    // Structure holding loaded image data and metadata.
    struct Data
    {
        std::vector<uint8_t> data;     // Raw image pixel data (bytes)
        uint32_t width = 1;            // Image width in pixels
        uint32_t height = 1;           // Image height in pixels
        uint32_t bitsPerPixel = 1;     // Bits per pixel
        uint32_t channelCount = 1;     // Number of color channels
        uint32_t stride = 1;           // Row pitch in bytes
        uint32_t slicePitch = 1;       // Total image size in bytes

        GUID wicPixelFormat = {};
        DXGI_FORMAT dxgiPixelFormat = DXGI_FORMAT_UNKNOWN;
    };

    explicit TextureLoader(const std::filesystem::path& fileName);

    // Return loaded data. noexcept: accessor only.
    const Data& GetData() const noexcept { return m_data; }

private:
    Data m_data;

    struct GUID_to_DXGI
    {
        GUID wic;
        DXGI_FORMAT dxgiFormat;
    };

    // Supported pixel format lookup table (defined in .cpp).
    static const std::vector<GUID_to_DXGI> s_lookupTable;
};
