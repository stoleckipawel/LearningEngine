// ============================================================================
// PrimitivePlane.h
// ----------------------------------------------------------------------------
// Flat quad on XZ plane primitive mesh.
// ============================================================================

#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Scene/Mesh.h"

class SPARKLE_ENGINE_API PrimitivePlane : public Mesh
{
  public:
	PrimitivePlane(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	// Generate the vertex data for the plane into outVertices. Override to change layout.
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;

	// Generate the index data for the plane into outIndices. Override to change layout.
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
