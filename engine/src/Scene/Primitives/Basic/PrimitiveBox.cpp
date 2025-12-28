#include "PCH.h"
#include "Primitives/Basic/PrimitiveBox.h"

PrimitiveBox::PrimitiveBox(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveBox::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	outVertices.clear();
	outVertices.reserve(24);
	const XMFLOAT4 whiteColor{1.0f, 1.0f, 1.0f, 1.0f};

	// Front face (z = +1)
	outVertices.push_back({{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}, whiteColor, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, whiteColor, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, whiteColor, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}, whiteColor, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	// Back face (z = -1)
	outVertices.push_back({{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, whiteColor, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f}, whiteColor, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f}, whiteColor, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, whiteColor, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f}});

	// Left face (x = -1)
	outVertices.push_back({{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, whiteColor, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});
	outVertices.push_back({{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f}, whiteColor, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});
	outVertices.push_back({{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, whiteColor, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});
	outVertices.push_back({{-1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}, whiteColor, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});

	// Right face (x = +1)
	outVertices.push_back({{1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}, whiteColor, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 1.0f}});
	outVertices.push_back({{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, whiteColor, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 1.0f}});
	outVertices.push_back({{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f}, whiteColor, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 1.0f}});
	outVertices.push_back({{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, whiteColor, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 1.0f}});

	// Top face (y = +1)
	outVertices.push_back({{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, whiteColor, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f}, whiteColor, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f}, whiteColor, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, whiteColor, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	// Bottom face (y = -1)
	outVertices.push_back({{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, whiteColor, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}, whiteColor, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}, whiteColor, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	outVertices.push_back({{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, whiteColor, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
}

void PrimitiveBox::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	outIndices.clear();
	outIndices.reserve(36);
	for (uint32_t face = 0; face < 6; ++face)
	{
		uint32_t base = face * 4;
		outIndices.push_back(base + 0);
		outIndices.push_back(base + 1);
		outIndices.push_back(base + 2);
		outIndices.push_back(base + 0);
		outIndices.push_back(base + 2);
		outIndices.push_back(base + 3);
	}
}
