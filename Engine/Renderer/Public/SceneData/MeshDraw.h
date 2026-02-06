// =============================================================================
// MeshDraw.h — Per-Instance Mesh Draw Command
// =============================================================================

#pragma once

#include "Renderer/Public/RendererAPI.h"

#include <DirectXMath.h>
#include <cstdint>

// =============================================================================
// MeshDraw
// =============================================================================

/// Per-instance draw command with transform and material reference.
/// meshPtr is opaque — used by GPUMeshCache to look up GPU buffers.
struct SPARKLE_RENDERER_API MeshDraw
{
	DirectX::XMFLOAT4X4 worldMatrix = {};
	DirectX::XMFLOAT3X4 worldInvTranspose = {};
	std::uint32_t materialId = 0;                 // Index into SceneView::materials[]
	const void* meshPtr = nullptr;                // Opaque handle for GPUMeshCache lookup
};
