
#pragma once
#include "TextureLoader.h"
#include "RHI.h"
#include "DescriptorHandle.h"

// Texture class manages loading, uploading, and resource creation for a 2D texture in Direct3D 12.
class Texture 
{
public:
    // Loads texture from disk and creates GPU resources.
    // Allocates an SRV descriptor via DescriptorHeapManager.
    Texture(const std::filesystem::path& fileName);
    // Destructor releases resources
    ~Texture();

    // Prevent copy and assignment to avoid double-free
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Returns the GPU descriptor handle for shader access
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_srvHandle.GetGPU(); }
    // Returns the CPU descriptor handle for descriptor heap management
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_srvHandle.GetCPU(); }

private:
    // Creates the committed resource for the texture on the GPU
    void CreateResource();
    // Uploads the texture data to the GPU resource
    void UploadToGPU();
    // Creates the shader resource view (SRV) for the texture
    void CreateShaderResourceView();
private:
    // GPU resource for the texture
    ComPtr<ID3D12Resource2> m_textureResource = nullptr;
    // Upload buffer resource for staging texture data
    ComPtr<ID3D12Resource2> m_uploadResource = nullptr;
    // RAII loader for image data
    std::unique_ptr<TextureLoader> m_loader;
    // Allocated SRV descriptor handle
    DescriptorHandle m_srvHandle;
    // Cached resource description for the texture
    D3D12_RESOURCE_DESC m_texResourceDesc = {};
};

