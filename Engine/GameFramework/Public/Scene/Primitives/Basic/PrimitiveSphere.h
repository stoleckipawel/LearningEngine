// ============================================================================
// PrimitiveSphere.h
// ----------------------------------------------------------------------------
// UV sphere primitive mesh with configurable tessellation.
// ============================================================================

#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"
#include "Scene/Mesh.h"

class SPARKLE_ENGINE_API PrimitiveSphere : public Mesh
{
  public:
	PrimitiveSphere(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	// Generate the vertex data for the sphere into outVertices.
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;

	// Generate the index data for the sphere into outIndices.
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
