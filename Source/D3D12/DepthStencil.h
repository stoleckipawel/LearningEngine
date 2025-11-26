#pragma once

#include "RHI.h"
#include "DescriptorHeapManager.h"


// DepthStencil manages a depth-stencil buffer resource and its view for Direct3D 12 rendering.
class DepthStencil 
{
public:
    // Constructs and initializes the depth stencil resource and view
    explicit DepthStencil(UINT DescriptorHandleIndex);

    // Destructor releases resources
    ~DepthStencil();

    // Deleted copy constructor and assignment operator to enforce unique ownership
    DepthStencil(const DepthStencil&) = delete;
    DepthStencil& operator=(const DepthStencil&) = delete;

    // Returns the GPU descriptor handle for shader access
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();
    // Returns the CPU descriptor handle for descriptor heap management
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(); 

    // Clears the depth stencil view
    void Clear();
private:
    // Creates the depth stencil resource on the GPU
    void CreateResource();
    // Releases the depth stencil resource
    void Release();
    // Creates the depth stencil view in the descriptor heap
    void CreateView();
private:
    ComPointer<ID3D12Resource> m_resource = nullptr;           // Depth stencil resource
    D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilDesc = {};     // DSV description
    UINT m_DescriptorHandleIndex = 0;                          // Index in descriptor heap
};

