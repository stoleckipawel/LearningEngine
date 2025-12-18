#pragma once

#include "Primitive.h"

// PrimitiveBox — a 3D cube/box primitive for rendering. Inherits from Primitive.
// Provides mesh data for a unit cube centered at the origin.
class PrimitiveBox : public Primitive {
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
