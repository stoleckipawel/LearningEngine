// ============================================================================
// Texture.h
// ----------------------------------------------------------------------------
// Manages loading, uploading, and GPU resource creation for 2D textures.
//
// USAGE:
//   Texture myTex("textures/diffuse.png");
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
#include "D3D12Rhi.h"
#include "D3D12DescriptorHandle.h"

using Microsoft::WRL::ComPtr;

class Texture
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Constructs a texture from file. Loads, uploads, and creates SRV.
	/// @param fileName Path to the texture file (absolute or relative to assets).
	explicit Texture(const std::filesystem::path& fileName);

	/// Releases the SRV descriptor slot.
	~Texture() noexcept;

	// Non-copyable: descriptor ownership cannot be shared
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Non-movable: prevents accidental transfer leading to double-free
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

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

	ComPtr<ID3D12Resource2> m_textureResource;      ///< GPU texture resource (default heap)
	ComPtr<ID3D12Resource2> m_uploadResource;       ///< Upload buffer (upload heap)
	std::unique_ptr<TextureLoader> m_loader;        ///< Texture loading helper
	D3D12DescriptorHandle m_srvHandle;              ///< SRV descriptor handle
	D3D12_RESOURCE_DESC m_texResourceDesc = {};     ///< Texture resource description
};
