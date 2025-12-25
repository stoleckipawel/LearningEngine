// Texture class manages loading, uploading, and resource creation for a 2D texture in Direct3D 12.
#pragma once

#include <filesystem>
#include <memory>
#include "TextureLoader.h"
#include "D3D12Rhi.h"
#include "D3D12DescriptorHandle.h"

using Microsoft::WRL::ComPtr;

// Note: `Texture` owns an SRV descriptor slot. The class is non-copyable to
// avoid double-freeing descriptor indices. Move semantics are provided to
// transfer ownership efficiently.
class Texture
{
  public:
	explicit Texture(const std::filesystem::path& fileName);
	~Texture() noexcept;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Strict ownership: disallow moves to prevent accidental transfer of
	// descriptor ownership which could lead to double-free or dangling
	// descriptors. The class remains non-copyable and non-movable.
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;

	// Descriptor handle accessors
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const noexcept { return m_srvHandle.GetGPU(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const noexcept { return m_srvHandle.GetCPU(); }

  private:
	void CreateResource();
	void UploadToGPU();
	void CreateShaderResourceView();

  private:
	ComPtr<ID3D12Resource2> m_textureResource;
	ComPtr<ID3D12Resource2> m_uploadResource;
	std::unique_ptr<TextureLoader> m_loader;
	D3D12DescriptorHandle m_srvHandle;
	D3D12_RESOURCE_DESC m_texResourceDesc = {};
};
