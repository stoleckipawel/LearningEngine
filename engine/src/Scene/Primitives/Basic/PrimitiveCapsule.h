// ============================================================================
// PrimitiveCapsule.h
// ----------------------------------------------------------------------------
// Capsule primitive mesh (cylinder with hemispherical caps).
// ============================================================================

#pragma once

#include "Scene/Mesh.h"

class PrimitiveCapsule : public Mesh
{
  public:
	PrimitiveCapsule(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
