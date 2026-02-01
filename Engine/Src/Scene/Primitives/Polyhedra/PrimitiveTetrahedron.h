// ============================================================================
// PrimitiveTetrahedron.h
// ----------------------------------------------------------------------------
// 4-faced polyhedron primitive mesh.
// ============================================================================

#pragma once

#include "Scene/Mesh.h"

class PrimitiveTetrahedron : public Mesh
{
  public:
	PrimitiveTetrahedron(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
