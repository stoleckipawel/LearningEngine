#pragma once

#include "ImageLoader.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"

class Texture 
{
public:
    ~Texture();
    void Initialize(const std::filesystem::path& imagePath, UINT DescriptorHandleIndex);
    void Release();
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();   

    void CreateResource();
    void UploadToGPU();
    void CreateSRV();
private:    
    ComPointer<ID3D12Resource2> m_textureResource = nullptr;
    ComPointer<ID3D12Resource2> m_uploadResource = nullptr;
    ImageLoader::ImageData m_textureData;
    UINT m_descriptorHandleIndex = 0;

    D3D12_RESOURCE_DESC m_texResourceDesc = {};
};

