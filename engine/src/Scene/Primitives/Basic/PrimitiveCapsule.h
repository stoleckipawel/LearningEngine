#pragma once

#include "../Primitive.h"

// PrimitiveCapsule — capsule aligned to Y axis.
// Default capsule fits within y in [-1, 1] with radius 0.5 and cylinder half-height 0.5.
class PrimitiveCapsule : public Primitive
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
