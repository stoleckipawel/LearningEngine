#include "PCH.h"
#include "Primitives/Basic/PrimitivePyramid.h"

#include "MathUtils.h"

using namespace DirectX;

PrimitivePyramid::PrimitivePyramid(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitivePyramid::GenerateGeometry(MeshData& outMeshData) const
{
	auto& outVertices = outMeshData.vertices;
	auto& outIndices = outMeshData.indices;

	outVertices.clear();
	outVertices.reserve(18);

	// Base square (y = -1)
	const DirectX::XMFLOAT3 b0{-1.0f, -1.0f, -1.0f};
	const DirectX::XMFLOAT3 b1{-1.0f, -1.0f, 1.0f};
	const DirectX::XMFLOAT3 b2{1.0f, -1.0f, 1.0f};
	const DirectX::XMFLOAT3 b3{1.0f, -1.0f, -1.0f};
	const DirectX::XMFLOAT3 apex{0.0f, 1.0f, 0.0f};

	// Base (2 triangles), outward is -Y
	const DirectX::XMFLOAT3 baseNormal{0.0f, -1.0f, 0.0f};
	const DirectX::XMFLOAT4 baseTangent{1.0f, 0.0f, 0.0f, 1.0f};

	// Base tri 0: b0, b2, b1 (CCW when viewed from -Y)
	outVertices.push_back({b0, {0.0f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}, baseNormal, baseTangent});
	outVertices.push_back({b2, {1.0f, 0.0f}, {0.6f, 0.6f, 0.6f, 1.0f}, baseNormal, baseTangent});
	outVertices.push_back({b1, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}, baseNormal, baseTangent});

	// Base tri 1: b0, b3, b2 (CCW when viewed from -Y)
	outVertices.push_back({b0, {0.0f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}, baseNormal, baseTangent});
	outVertices.push_back({b3, {1.0f, 1.0f}, {0.7f, 0.7f, 0.7f, 1.0f}, baseNormal, baseTangent});
	outVertices.push_back({b2, {1.0f, 0.0f}, {0.6f, 0.6f, 0.6f, 1.0f}, baseNormal, baseTangent});

	// Side faces (4 triangles). Duplicate vertices for flat shading.
	const struct Face
	{
		DirectX::XMFLOAT3 a, b, c;
	} faces[4] = {
	    {b0, b1, apex},
	    {b1, b2, apex},
	    {b2, b3, apex},
	    {b3, b0, apex},
	};

	for (int f = 0; f < 4; ++f)
	{
		DirectX::XMFLOAT3 e1 = MathUtils::Sub(faces[f].b, faces[f].a);
		DirectX::XMFLOAT3 e2 = MathUtils::Sub(faces[f].c, faces[f].a);
		DirectX::XMFLOAT3 normal = MathUtils::Normalize3(MathUtils::Cross(e1, e2));

		// Tangent: project e1 to plane and normalize
		DirectX::XMFLOAT3 t3 = MathUtils::Normalize3(e1);
		DirectX::XMFLOAT4 tangent{t3.x, t3.y, t3.z, 1.0f};

		auto uvA = DirectX::XMFLOAT2{0.0f, 1.0f};
		auto uvB = DirectX::XMFLOAT2{1.0f, 1.0f};
		auto uvC = DirectX::XMFLOAT2{0.5f, 0.0f};

		outVertices.push_back({faces[f].a, uvA, {fabsf(normal.x), fabsf(normal.y), fabsf(normal.z), 1.0f}, normal, tangent});
		outVertices.push_back({faces[f].b, uvB, {fabsf(normal.x), fabsf(normal.y), fabsf(normal.z), 1.0f}, normal, tangent});
		outVertices.push_back({faces[f].c, uvC, {fabsf(normal.x), fabsf(normal.y), fabsf(normal.z), 1.0f}, normal, tangent});
	}

	// Vertices are already expanded per-triangle.
	outIndices.clear();
	outIndices.reserve(18);
	for (uint32_t i = 0; i < 18; ++i)
		outIndices.push_back(i);
}
