#pragma once

#include "Renderer/Public/RendererAPI.h"

#include <DirectXMath.h>

/**
 * @brief Directional light data for scene lighting.
 *
 * Represents an infinitely distant light source (like the sun) that
 * illuminates the scene uniformly from a single direction.
 */
struct SPARKLE_RENDERER_API DirectionalLight
{
	DirectX::XMFLOAT3 direction = {0.0f, -1.0f, 0.0f};  // Light direction (normalized)
	float intensity = 1.0f;                              // Light intensity
	DirectX::XMFLOAT3 color = {1.0f, 1.0f, 1.0f};       // Light color (RGB)
	float _padding = 0.0f;                               // Alignment padding
};
