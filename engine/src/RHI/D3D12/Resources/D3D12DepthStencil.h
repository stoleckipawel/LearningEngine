// ============================================================================
// D3D12DepthStencil.h
// ----------------------------------------------------------------------------
// Manages a GPU depth-stencil resource and its DSV descriptor.
//
// USAGE:
//   D3D12DepthStencil depthStencil;
//   depthStencil.SetWriteState();
//   depthStencil.Clear();
//   // ... render with depth testing ...
//   depthStencil.SetReadState();
//
// DESIGN:
//   - Uses committed default-heap resource with optimized clear value
//   - Owns the DSV descriptor handle and GPU resource exclusively
//   - All public accessors are const noexcept and non-mutating
//
// NOTES:
//   - Copy/move deleted to enforce unique ownership semantics
//   - Resize requires creating a new instance
// ============================================================================

#pragma once

#include "D3D12Rhi.h"
#include "D3D12DescriptorHandle.h"

using Microsoft::WRL::ComPtr;

class D3D12DepthStencil
{
  public:
	// Constructs and initializes the depth stencil resource and view
	D3D12DepthStencil();

	// Destructor releases resources and descriptor handle
	~D3D12DepthStencil() noexcept;

	// Deleted copy/move to enforce unique ownership
	D3D12DepthStencil(const D3D12DepthStencil&) = delete;
	D3D12DepthStencil& operator=(const D3D12DepthStencil&) = delete;
	D3D12DepthStencil(D3D12DepthStencil&&) = delete;
	D3D12DepthStencil& operator=(D3D12DepthStencil&&) = delete;

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
	D3D12_DEPTH_STENCIL_VIEW_DESC m_depthStencilDesc = {};  // DSV description
	D3D12DescriptorHandle m_dsvHandle;                      // Allocated DSV descriptor handle
};
