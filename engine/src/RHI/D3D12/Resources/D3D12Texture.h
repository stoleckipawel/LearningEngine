// ============================================================================
// D3D12Texture.h
// ----------------------------------------------------------------------------
// Manages loading, uploading, and GPU resource creation for 2D textures.
//
// USAGE:
//   D3D12Texture myTex("textures/diffuse.png");
//   auto gpuHandle = myTex.GetGPUHandle();  // Bind to shader
//
// DESIGN:
//   - Loads via TextureLoader (supports common formats)
//   - Creates D3D12 committed resource and upload buffer
//   - Allocates SRV descriptor from engine's descriptor heap
//
// OWNERSHIP:
//   - Owns an SRV descriptor slot; non-copyable/non-movable to prevent
//     double-free of descriptor indices
//   - Destructor frees the descriptor slot
//
// NOTES:
//   - Constructor performs load + upload synchronously
//   - Upload buffer kept alive until command list execution completes
// ============================================================================

#pragma once

#include <filesystem>
#include <memory>
#include "TextureLoader.h"
#include "D3D12DescriptorHandle.h"
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class AssetSystem;
class D3D12DescriptorHeapManager;
class D3D12Rhi;

class D3D12Texture
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Constructs a texture from file. Loads, uploads, and creates SRV.
	/// @param assetSystem Reference to the asset system for path resolution.
	/// @param rhi Reference to the D3D12 RHI for device access.
	/// @param fileName Path to the texture file (absolute or relative to assets).
	/// @param descriptorHeapManager Reference to the descriptor heap manager for SRV allocation.
	D3D12Texture(
	    const AssetSystem& assetSystem,
	    D3D12Rhi& rhi,
	    const std::filesystem::path& fileName,
	    D3D12DescriptorHeapManager& descriptorHeapManager);

	/// Releases the SRV descriptor slot.
	~D3D12Texture() noexcept;

	// Non-copyable: descriptor ownership cannot be shared
	D3D12Texture(const D3D12Texture&) = delete;
	D3D12Texture& operator=(const D3D12Texture&) = delete;

	// Non-movable: prevents accidental transfer leading to double-free
	D3D12Texture(D3D12Texture&&) = delete;
	D3D12Texture& operator=(D3D12Texture&&) = delete;

	// ========================================================================
	// Accessors
	// ========================================================================

	/// Returns the GPU descriptor handle for shader binding.
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_srvHandle.GetGPU(); }

	/// Returns the CPU descriptor handle (for copy operations).
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_srvHandle.GetCPU(); }

  private:
	// ------------------------------------------------------------------------
	// Initialization Helpers
	// ------------------------------------------------------------------------

	/// Creates the committed GPU texture resource.
	void CreateResource();

	/// Copies texture data from CPU to GPU via upload buffer.
	void UploadToGPU();

	/// Allocates SRV descriptor and creates the view.
	void CreateShaderResourceView();

	// ------------------------------------------------------------------------
	// Resources
	// ------------------------------------------------------------------------

	D3D12Rhi& m_rhi;                                                ///< RHI reference
	ComPtr<ID3D12Resource2> m_textureResource;                      ///< GPU texture resource (default heap)
	ComPtr<ID3D12Resource2> m_uploadResource;                       ///< Upload buffer (upload heap)
	std::unique_ptr<TextureLoader> m_loader;                        ///< Texture loading helper
	D3D12DescriptorHandle m_srvHandle;                              ///< SRV descriptor handle
	D3D12_RESOURCE_DESC m_texResourceDesc = {};                     ///< Texture resource description
	D3D12DescriptorHeapManager* m_descriptorHeapManager = nullptr;  ///< Descriptor heap manager reference
};
