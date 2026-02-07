// =============================================================================
// MaterialData.h — PBR Material Parameters
// =============================================================================

#pragma once

#include "Renderer/Public/RendererAPI.h"
#include "D3D12/Resources/D3D12ConstantBufferData.h"

#include <DirectXMath.h>
#include <cstdint>

struct MaterialDesc;

// =============================================================================
// MaterialData
// =============================================================================

/// PBR material parameters. No GPU handles — albedoTextureIdx indexes
/// into the texture array managed by TextureManager.
struct SPARKLE_RENDERER_API MaterialData
{
	DirectX::XMFLOAT4 baseColor = {1.0f, 1.0f, 1.0f, 1.0f};
	float metallic = 0.0f;
	float roughness = 0.5f;
	float f0 = 0.04f;                             // Fresnel reflectance at normal incidence
	std::uint32_t albedoTextureIdx = UINT32_MAX;  // UINT32_MAX = no texture bound

	/// Creates a MaterialData from a CPU-side MaterialDesc.
	[[nodiscard]] static MaterialData FromDesc(const MaterialDesc& desc);

	/// Builds the GPU constant buffer data from this material.
	[[nodiscard]] PerObjectPSConstantBufferData ToPerObjectPSData() const
	{
		PerObjectPSConstantBufferData data{};
		data.BaseColor = baseColor;
		data.Metallic = metallic;
		data.Roughness = roughness;
		data.F0 = f0;
		return data;
	}
};
