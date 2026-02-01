// ============================================================================
// PrimitiveBox.h
// ----------------------------------------------------------------------------
// Unit cube primitive mesh centered at origin.
// ============================================================================

#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Scene/Mesh.h"

class SPARKLE_ENGINE_API PrimitiveBox : public Mesh
{
  public:
	// Construct a new PrimitiveBox object. Parameters default to identity (no transform).
	PrimitiveBox(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	// Generate the vertex data for the box into outVertices. Override to change layout.
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;

	// Generate the index data for the box into outIndices. Override to change layout.
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
