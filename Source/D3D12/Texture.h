#pragma once

#include "ImageLoader.h"
#include "RHI.h"


class Texture 
{
public:
    ~Texture();
    void Load(const std::filesystem::path& imagePath,
              ID3D12GraphicsCommandList* cmdList,
              ID3D12DescriptorHeap* srvHeap,
              UINT m_DescriptorHandleIndex);
    void Release();

    UINT ComputeMipCount(UINT width, UINT height);

    void CreateResource();
    void UploadToGPU(ID3D12GraphicsCommandList* cmdList);
    void CreateSRV(ID3D12DescriptorHeap* srvHeap, UINT m_DescriptorHandleIndex);

    ComPointer<ID3D12Resource> textureResource = nullptr;
    ComPointer<ID3D12Resource> uploadResource = nullptr;

    ImageLoader::ImageData textureData;
    UINT m_DescriptorHandleIndex = 0;
};

