#pragma once

#include "RHI.h"
#include "DescriptorHandle.h"

using Microsoft::WRL::ComPtr;

// DepthStencil manages a GPU depth-stencil resource and its DSV descriptor.
// Design notes:
// - Uses a committed default-heap resource with an optimized clear value.
// - The class owns the DSV descriptor handle and the GPU resource exclusively.
// - All public accessors are `const noexcept` and non-mutating.
// - Copy/move are deleted to enforce unique ownership semantics.

// DepthStencil manages a depth-stencil buffer resource and its view for Direct3D 12 rendering.
class DepthStencil 
{
public:
    // Constructs and initializes the depth stencil resource and view
    DepthStencil();

    // Destructor releases resources and descriptor handle
    ~DepthStencil() noexcept;

    // Deleted copy/move to enforce unique ownership
    DepthStencil(const DepthStencil&) = delete;
    DepthStencil& operator=(const DepthStencil&) = delete;
    DepthStencil(DepthStencil&&) = delete;
    DepthStencil& operator=(DepthStencil&&) = delete;

    // Returns the GPU descriptor handle for shader access
    // Returns the GPU descriptor handle for shader binding (non-owning)
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_dsvHandle.GetGPU(); }
    // Returns the CPU descriptor handle for descriptor heap management (non-owning)
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_dsvHandle.GetCPU(); }

    // Clears the depth stencil view
    void Clear() noexcept;
    // Transition depth buffer to writable state before rendering
    void SetWriteState() noexcept;
    // Transition depth buffer to readable state after rendering
    void SetReadState() noexcept;
    
    // Internal helper: returns underlying resource. Returns const reference to avoid copies.
    const ComPtr<ID3D12Resource2>& GetResource() const noexcept { return m_resource; }
private:
    // Creates the depth stencil resource on the GPU
    void CreateResource();
    // Creates the depth stencil view in the descriptor heap
    void CreateDepthStencilView();
private:
    ComPtr<ID3D12Resource2> m_resource;                     // Depth stencil resource
    D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilDesc = {};     // DSV description
    DescriptorHandle m_dsvHandle;                              // Allocated DSV descriptor handle
};

