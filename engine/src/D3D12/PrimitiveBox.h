#pragma once
#include "D3D12/Primitive.h"

/**
 * @brief PrimitiveBox - Cube/Box implementation of Primitive
 */
class PrimitiveBox : public Primitive {
public:
    PrimitiveBox();
protected:
    void GenerateVertices(std::vector<Vertex>& outVertices) const override;
    void GenerateIndices(std::vector<DWORD>& outIndices) const override;
};
