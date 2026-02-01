#include "PCH.h"
#include "Primitives/Polyhedra/PrimitiveTetrahedron.h"

using namespace DirectX;

PrimitiveTetrahedron::PrimitiveTetrahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveTetrahedron::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	outVertices = {
	    {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {1, 0, 0, 1}, {0.57735f, 0.57735f, 0.57735f}, {-0.7071f, 0.0f, 0.7071f, 1.0f}},
	    {{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}, {0, 1, 0, 1}, {-0.57735f, -0.57735f, 0.57735f}, {-0.7071f, 0.0f, -0.7071f, 1.0f}},
	    {{-1.0f, 1.0f, -1.0f}, {0.5f, 1.0f}, {0, 0, 1, 1}, {-0.57735f, 0.57735f, -0.57735f}, {0.7071f, 0.0f, -0.7071f, 1.0f}},
	    {{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, {1, 1, 0, 1}, {0.57735f, -0.57735f, -0.57735f}, {0.7071f, 0.0f, 0.7071f, 1.0f}}};
}

void PrimitiveTetrahedron::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	outIndices = {0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2};
}
