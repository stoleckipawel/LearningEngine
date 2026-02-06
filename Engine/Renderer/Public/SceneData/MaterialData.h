// =============================================================================
// MaterialData.h — PBR Material Parameters
// =============================================================================

#pragma once

#include "Renderer/Public/RendererAPI.h"

#include <DirectXMath.h>
#include <cstdint>

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
	float f0 = 0.04f;                                           // Fresnel reflectance at normal incidence
	std::uint32_t albedoTextureIdx = UINT32_MAX;                 // UINT32_MAX = no texture bound
};
