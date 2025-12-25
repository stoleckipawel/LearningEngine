#pragma once

#include "Primitive.h"

// PrimitivePlane — a simple 2D plane primitive for rendering (XZ plane). Inherits from Primitive.
class PrimitivePlane : public Primitive
{
  public:
	PrimitivePlane(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	// Generate the vertex data for the plane into outVertices. Override to change layout.
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;

	// Generate the index data for the plane into outIndices. Override to change layout.
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
