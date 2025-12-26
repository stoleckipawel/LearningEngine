#pragma once

#include "Primitive.h"

// PrimitiveSphere — a UV sphere primitive for rendering. Inherits from Primitive.
class PrimitiveSphere : public Primitive
{
  public:
    PrimitiveSphere(
        const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
        const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

  protected:
    // Generate the vertex data for the sphere into outVertices.
    void GenerateVertices(std::vector<Vertex>& outVertices) const override;

    // Generate the index data for the sphere into outIndices.
    void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
