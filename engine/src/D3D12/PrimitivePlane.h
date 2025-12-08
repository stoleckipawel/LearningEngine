#pragma once
#include "D3D12/Primitive.h"

/**
 * @brief PrimitivePlane - Plane implementation of Primitive
 */
class PrimitivePlane : public Primitive 
{
public:
    PrimitivePlane();
protected:
    void GenerateVertices(std::vector<Vertex>& outVertices) const override;
    void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
