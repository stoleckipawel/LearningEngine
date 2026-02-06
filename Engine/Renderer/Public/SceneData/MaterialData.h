#pragma once

#include "Renderer/Public/RendererAPI.h"

#include <DirectXMath.h>
#include <cstdint>

/**
 * @brief Material data for PBR rendering.
 *
 * Pure data - no GPU handles. albedoTextureIdx is an index into
 * the texture array managed by TextureManager.
 */
struct SPARKLE_RENDERER_API MaterialData
{
	DirectX::XMFLOAT4 baseColor = {1.0f, 1.0f, 1.0f, 1.0f};  // RGBA base/albedo color
	float metallic = 0.0f;                                    // Metallic factor [0, 1]
	float roughness = 0.5f;                                   // Roughness factor [0, 1]
	float f0 = 0.04f;                                         // Fresnel reflectance at normal incidence
	uint32_t albedoTextureIdx = UINT32_MAX;                   // Index into texture array, UINT32_MAX = no texture
};
