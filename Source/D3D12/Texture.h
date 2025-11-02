#pragma once

#include "ImageLoader.h"
#include "RHI.h"
#include "DescriptorHeapManager.h"

class Texture 
{
public:
    ~Texture();
    void Initialize(const std::filesystem::path& imagePath, ID3D12GraphicsCommandList* cmdList, UINT m_DescriptorHandleIndex);
    void Release();
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();   

    void CreateResource();
    void UploadToGPU(ID3D12GraphicsCommandList* cmdList);
    void CreateSRV();




private:

private:    
    ComPointer<ID3D12Resource> m_textureResource = nullptr;
    ComPointer<ID3D12Resource> m_uploadResource = nullptr;
    ImageLoader::ImageData m_textureData;
    UINT m_DescriptorHandleIndex = 0;
};

