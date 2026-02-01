// ============================================================================
// PrimitiveIcosphere.h
// ----------------------------------------------------------------------------
// Subdivided icosahedron with uniform triangles.
// ============================================================================

#pragma once

#include "Scene/Mesh.h"

class PrimitiveIcosphere : public Mesh
{
  public:
	PrimitiveIcosphere(
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
