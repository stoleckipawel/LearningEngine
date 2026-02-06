#pragma once

#include "Renderer/Public/RendererAPI.h"

#include <DirectXMath.h>
#include <cstdint>

/**
 * @brief Per-instance mesh draw command.
 *
 * Contains transform data and material reference for a single mesh instance.
 * meshPtr is an opaque pointer used by GPUMeshCache to look up GPU resources.
 */
struct SPARKLE_RENDERER_API MeshDraw
{
	DirectX::XMFLOAT4X4 worldMatrix = {};            // Local -> World transform
	DirectX::XMFLOAT3X4 worldInvTranspose = {};      // Normal matrix (inverse-transpose)
	uint32_t materialId = 0;                          // Index into SceneView::materials[]
	const void* meshPtr = nullptr;                    // Opaque pointer for GPUMeshCache lookup
};
