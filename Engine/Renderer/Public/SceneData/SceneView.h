#pragma once

#include "Renderer/Public/RendererAPI.h"
#include "Renderer/Public/SceneData/DirectionalLight.h"
#include "Renderer/Public/SceneData/MaterialData.h"
#include "Renderer/Public/SceneData/MeshDraw.h"

#include <DirectXMath.h>
#include <cstdint>
#include <vector>

/**
 * @brief Pure-data structure representing the scene for rendering.
 *
 * SceneView crosses the GameFramework → Renderer boundary.
 * Contains no D3D12 types - only POD data that can be serialized, logged, or replayed.
 *
 * Key Constraints:
 * - NO D3D12 types
 * - NO GPU handles
 * - Pure data only
 */
struct SPARKLE_RENDERER_API SceneView
{
	// ─────────────────────────────────────────────────────────────────────────
	// Camera Data
	// ─────────────────────────────────────────────────────────────────────────
	DirectX::XMFLOAT4X4 viewMatrix = {};         // World -> View
	DirectX::XMFLOAT4X4 projMatrix = {};         // View -> Clip
	DirectX::XMFLOAT4X4 viewProjMatrix = {};     // World -> Clip (precomputed)
	DirectX::XMFLOAT3 cameraPosition = {};       // World-space camera position
	float nearZ = 0.1f;                           // Near clip plane
	DirectX::XMFLOAT3 cameraDirection = {};      // World-space camera forward vector
	float farZ = 1000.0f;                         // Far clip plane

	// ─────────────────────────────────────────────────────────────────────────
	// Viewport
	// ─────────────────────────────────────────────────────────────────────────
	uint32_t width = 0;
	uint32_t height = 0;

	// ─────────────────────────────────────────────────────────────────────────
	// Lighting
	// ─────────────────────────────────────────────────────────────────────────
	DirectionalLight sunLight = {};

	// ─────────────────────────────────────────────────────────────────────────
	// Draw Commands
	// ─────────────────────────────────────────────────────────────────────────
	std::vector<MeshDraw> meshDraws;             // One per mesh instance
	std::vector<MaterialData> materials;         // All materials referenced by meshDraws
};
