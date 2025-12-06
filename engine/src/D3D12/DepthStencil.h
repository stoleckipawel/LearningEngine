#pragma once

#include "RHI.h"

// DepthStencil manages a depth-stencil buffer resource and its view for Direct3D 12 rendering.
class DepthStencil 
{
public:
    // Constructs and initializes the depth stencil resource and view
    explicit DepthStencil(UINT DescriptorHandleIndex);

    // Destructor Resets resources
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
    // Transition depth buffer to writable state before rendering
    void SetWriteState();
    // Transition depth buffer to readable state after rendering
    void SetReadState();
    
    // Internal helper: returns underlying resource (if ever needed internally)
    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return m_resource; }
private:
    // Creates the depth stencil resource on the GPU
    void CreateResource();
    // Creates the depth stencil view in the descriptor heap
    void CreateDepthStencilView();
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;                     // Depth stencil resource
    D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilDesc = {};     // DSV description
    UINT m_DescriptorHandleIndex = 0;                          // Index in descriptor heap
};

