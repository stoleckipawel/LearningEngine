#pragma once

#include "RHI.h"
#include "DescriptorHeapManager.h"

class DepthStencil 
{
public:
    ~DepthStencil();
    void Initialize(UINT DescriptorHandleIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(); 
    void Clear();
private:    
    void CreateResource();
    void Release();
    void CreateView();
private:
    ComPointer<ID3D12Resource> m_resource = nullptr;
    D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilDesc = {};
    UINT m_DescriptorHandleIndex = 0;
};

