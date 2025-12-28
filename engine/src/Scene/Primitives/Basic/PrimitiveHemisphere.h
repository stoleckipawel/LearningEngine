#pragma once

#include "../Primitive.h"

// PrimitiveHemisphere — upper hemisphere (y >= 0) with a flat base cap.
class PrimitiveHemisphere : public Primitive
{
  public:
	PrimitiveHemisphere(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
	void GenerateVertices(std::vector<Vertex>& outVertices) const override;
	void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
