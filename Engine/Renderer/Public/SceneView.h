#pragma once

#include "Renderer/Public/RendererAPI.h"

/**
 * @brief Pure-data structure representing the scene for rendering.
 *
 * SceneView crosses the GameFramework → Renderer boundary.
 * Contains no D3D12 types - only POD data that can be serialized.
 *
 * TODO: Expand in Phase 2 with camera, lighting, and mesh draw data.
 */
struct SPARKLE_RENDERER_API SceneView
{
	// Phase 2 will add:
	// - Camera matrices (view, projection, viewProj)
	// - Camera position/direction
	// - Viewport dimensions
	// - Sun light data
	// - MeshDraw array
	// - MaterialData array

	// Placeholder for MVP
	uint32_t width = 0;
	uint32_t height = 0;
};
