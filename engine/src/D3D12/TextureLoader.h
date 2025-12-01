#pragma once

/**
 * @brief TextureLoader provides static methods for loading images from disk using WIC.
 */
class TextureLoader
{
public:
    /**
     * @brief Structure holding loaded image data and metadata.
     */
    struct Data
    {
        std::vector<char> data;         // Raw image pixel data
        uint32_t width = 1;             // Image width in pixels
        uint32_t height = 1;            // Image height in pixels
        uint32_t bitsPerPixel = 1;      // Bits per pixel
        uint32_t channelCount = 1;      // Number of color channels
        uint32_t stride = 1;            // Row pitch in bytes
        uint32_t slicePitch = 1;        // Total image size in bytes

        GUID wicPixelFormat = {};
        DXGI_FORMAT dxgiPixelFormat = DXGI_FORMAT_UNKNOWN;
    };

    /**
     * @brief Loads an image from disk and populates Data.
     * @param fileName Path to the image file.
     */
    TextureLoader(const std::filesystem::path& fileName);
    ~TextureLoader();

    /**
     * @brief Returns loaded image data.
     */
    const Data& GetData() const { return m_data; }


private:
    Data m_data;

    struct GUID_to_DXGI
    {
        GUID wic;
        DXGI_FORMAT dxgiFormat;
    };
    static const std::vector<GUID_to_DXGI> s_lookupTable;
};
