#include "PCH.h"
#include "Primitives/Polyhedra/PrimitiveOctahedron.h"

PrimitiveOctahedron::PrimitiveOctahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Primitive(translation, rotation, scale)
{
}

void PrimitiveOctahedron::GenerateVertices(std::vector<Vertex>& outVertices) const
{
    outVertices = {
        {{1.0f, 0.0f, 0.0f}, {0.0f, 0.5f}, {1, 0, 0, 1}},   // 0
        {{-1.0f, 0.0f, 0.0f}, {1.0f, 0.5f}, {0, 1, 0, 1}},  // 1
        {{0.0f, 1.0f, 0.0f}, {0.5f, 0.0f}, {0, 0, 1, 1}},   // 2
        {{0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}, {1, 1, 0, 1}},  // 3
        {{0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}, {1, 0, 1, 1}},   // 4
        {{0.0f, 0.0f, -1.0f}, {0.5f, 0.5f}, {0, 1, 1, 1}}   // 5
    };
}

void PrimitiveOctahedron::GenerateIndices(std::vector<DWORD>& outIndices) const
{
    outIndices = {
        // Upper pyramid
        2, 4, 0,
        2, 0, 5,
        2, 5, 1,
        2, 1, 4,
        // Lower pyramid
        3, 0, 4,
        3, 5, 0,
        3, 1, 5,
        3, 4, 1};
}
