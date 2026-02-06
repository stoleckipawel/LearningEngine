#include "PCH.h"
#include "Primitives/Polyhedra/PrimitiveOctahedron.h"

using namespace DirectX;

PrimitiveOctahedron::PrimitiveOctahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveOctahedron::GenerateGeometry(MeshData& outMeshData) const
{
	auto& outVertices = outMeshData.vertices;
	auto& outIndices = outMeshData.indices;

	outVertices = {
	    {{1.0f, 0.0f, 0.0f}, {0.0f, 0.5f}, {1, 0, 0, 1}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},    // 0
	    {{-1.0f, 0.0f, 0.0f}, {1.0f, 0.5f}, {0, 1, 0, 1}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 1
	    {{0.0f, 1.0f, 0.0f}, {0.5f, 0.0f}, {0, 0, 1, 1}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},    // 2
	    {{0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}, {1, 1, 0, 1}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},  // 3
	    {{0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}, {1, 0, 1, 1}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},    // 4
	    {{0.0f, 0.0f, -1.0f}, {0.5f, 0.5f}, {0, 1, 1, 1}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}   // 5
	};

	outIndices = {
	    // Upper pyramid
	    2,
	    4,
	    0,
	    2,
	    0,
	    5,
	    2,
	    5,
	    1,
	    2,
	    1,
	    4,
	    // Lower pyramid
	    3,
	    0,
	    4,
	    3,
	    5,
	    0,
	    3,
	    1,
	    5,
	    3,
	    4,
	    1};
}
