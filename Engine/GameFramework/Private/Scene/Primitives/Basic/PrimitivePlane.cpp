#include "PCH.h"
#include "Primitives/Basic/PrimitivePlane.h"

using namespace DirectX;

PrimitivePlane::PrimitivePlane(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitivePlane::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	// Simple 2x2 plane in XZ
	outVertices = {
	    // position, uv, color, normal, tangent
	    {{-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},  // 0
	    {{-1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},   // 1
	    {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},     // 2
	    {{1.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},    // 3
	};
}

void PrimitivePlane::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	outIndices = {0, 1, 2, 0, 2, 3};
}
