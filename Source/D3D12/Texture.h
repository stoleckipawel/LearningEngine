#pragma once

#include "ImageLoader.h"
#include "RHI.h"

// Texture class manages loading, uploading, and resource creation for a 2D texture in Direct3D 12.
class Texture 
{
public:
    // Loads texture from disk and creates GPU resources
    Texture(const std::filesystem::path& imagePath, UINT descriptorHandleIndex);
    // Destructor releases resources
    ~Texture();

    // Prevent copy and assignment to avoid double-free
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Returns the GPU descriptor handle for shader access
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;
    // Returns the CPU descriptor handle for descriptor heap management
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;

private:
    // Creates the committed resource for the texture on the GPU
    void CreateResource();
    // Releases all GPU resources associated with the texture
    void Release();    
    // Uploads the texture data to the GPU resource
    void UploadToGPU();
    // Creates the shader resource view (SRV) for the texture
    void CreateSRV();
private:    
    // GPU resource for the texture
    ComPointer<ID3D12Resource2> m_textureResource = nullptr;
    // Upload buffer resource for staging texture data
    ComPointer<ID3D12Resource2> m_uploadResource = nullptr;
    // Loaded image data from disk
    ImageLoader::ImageData m_textureData;
    // Index in the descriptor heap
    UINT m_descriptorHandleIndex = 0;
    // Cached resource description for the texture
    D3D12_RESOURCE_DESC m_texResourceDesc = {};
};

