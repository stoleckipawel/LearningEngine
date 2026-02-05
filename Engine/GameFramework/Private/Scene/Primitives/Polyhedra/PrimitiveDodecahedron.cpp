#include "PCH.h"
#include "Primitives/Polyhedra/PrimitiveDodecahedron.h"

#include "MathUtils.h"

#include <array>

using namespace DirectX;

namespace
{
	// Shared base icosahedron definition (same indexing as PrimitiveIcosahedron)
	static void BuildIcosahedron(std::array<DirectX::XMFLOAT3, 12>& outVerts, std::array<uint32_t, 60>& outIdx)
	{
		const float phi = (1.0f + sqrtf(5.0f)) * 0.5f;
		outVerts = {
		    DirectX::XMFLOAT3{-1.0f, phi, 0.0f},
		    DirectX::XMFLOAT3{1.0f, phi, 0.0f},
		    DirectX::XMFLOAT3{-1.0f, -phi, 0.0f},
		    DirectX::XMFLOAT3{1.0f, -phi, 0.0f},
		    DirectX::XMFLOAT3{0.0f, -1.0f, phi},
		    DirectX::XMFLOAT3{0.0f, 1.0f, phi},
		    DirectX::XMFLOAT3{0.0f, -1.0f, -phi},
		    DirectX::XMFLOAT3{0.0f, 1.0f, -phi},
		    DirectX::XMFLOAT3{phi, 0.0f, -1.0f},
		    DirectX::XMFLOAT3{phi, 0.0f, 1.0f},
		    DirectX::XMFLOAT3{-phi, 0.0f, -1.0f},
		    DirectX::XMFLOAT3{-phi, 0.0f, 1.0f},
		};

		for (auto& v : outVerts)
			v = MathUtils::Normalize3(v);

		outIdx = {
		    0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,  11, 10, 2,  10, 7, 6, 7, 1, 8,
		    3, 9,  4, 3, 4, 2, 3, 2, 6, 3, 6, 8,  3, 8,  9,  4, 9, 5, 2, 4,  11, 6,  2,  10, 8,  6, 7, 9, 8, 1,
		};
	}
}  // namespace

PrimitiveDodecahedron::PrimitiveDodecahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveDodecahedron::GenerateGeometry(MeshData& outMeshData) const
{
	auto& outVertices = outMeshData.vertices;
	auto& outIndices = outMeshData.indices;

	// Build dual of icosahedron:
	// - Each icosa face center => a dodeca vertex (20)
	// - Each icosa vertex connects 5 faces => a dodeca face (pentagon)
	std::array<DirectX::XMFLOAT3, 12> icoVerts;
	std::array<uint32_t, 60> icoIdx;
	BuildIcosahedron(icoVerts, icoIdx);

	// Face centers
	std::array<DirectX::XMFLOAT3, 20> faceCenters;
	for (int f = 0; f < 20; ++f)
	{
		uint32_t i0 = icoIdx[f * 3 + 0];
		uint32_t i1 = icoIdx[f * 3 + 1];
		uint32_t i2 = icoIdx[f * 3 + 2];
		DirectX::XMFLOAT3 c = MathUtils::Mul(MathUtils::Add(MathUtils::Add(icoVerts[i0], icoVerts[i1]), icoVerts[i2]), 1.0f / 3.0f);
		faceCenters[f] = MathUtils::Normalize3(c);
	}

	outVertices.clear();
	outVertices.reserve(faceCenters.size());
	for (auto& p : faceCenters)
	{
		DirectX::XMFLOAT3 n = MathUtils::Normalize3(p);
		DirectX::XMFLOAT2 uv = MathUtils::SphericalUV(n);

		float phiAng = std::atan2(n.z, n.x);
		DirectX::XMFLOAT3 tangent{-sinf(phiAng), 0.0f, cosf(phiAng)};
		tangent = MathUtils::Normalize3(tangent);

		DirectX::XMFLOAT4 color{fabsf(n.x), fabsf(n.y), fabsf(n.z), 1.0f};
		outVertices.push_back({n, uv, color, n, {tangent.x, tangent.y, tangent.z, 1.0f}});
	}

	// Map each icosa vertex -> incident faces
	std::array<std::vector<int>, 12> incidentFaces;
	for (int i = 0; i < 12; ++i)
		incidentFaces[i].clear();

	for (int f = 0; f < 20; ++f)
	{
		uint32_t i0 = icoIdx[f * 3 + 0];
		uint32_t i1 = icoIdx[f * 3 + 1];
		uint32_t i2 = icoIdx[f * 3 + 2];
		incidentFaces[i0].push_back(f);
		incidentFaces[i1].push_back(f);
		incidentFaces[i2].push_back(f);
	}

	outIndices.clear();
	outIndices.reserve(12 * 3 * 3);  // 12 pentagons -> 36 triangles

	for (int v = 0; v < 12; ++v)
	{
		auto& faces = incidentFaces[v];
		if (faces.size() != 5)
			continue;

		DirectX::XMFLOAT3 n = MathUtils::Normalize3(icoVerts[v]);
		DirectX::XMFLOAT3 up = (fabsf(n.y) < 0.9f) ? DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f} : DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 tangent = MathUtils::Normalize3(MathUtils::Cross(up, n));
		DirectX::XMFLOAT3 bitangent = MathUtils::Cross(n, tangent);

		struct FaceAngle
		{
			int faceIndex;
			float angle;
		};
		std::array<FaceAngle, 5> ordered;

		for (int i = 0; i < 5; ++i)
		{
			int f = faces[i];
			// face center is the dodeca vertex position (index f)
			// project onto tangent plane at n
			DirectX::XMFLOAT3 c = MathUtils::Normalize3(
			    MathUtils::Mul(
			        MathUtils::Add(MathUtils::Add(icoVerts[icoIdx[f * 3 + 0]], icoVerts[icoIdx[f * 3 + 1]]), icoVerts[icoIdx[f * 3 + 2]]),
			        1.0f / 3.0f));
			DirectX::XMFLOAT3 d = MathUtils::Sub(c, MathUtils::Mul(n, MathUtils::Dot(c, n)));
			float x = MathUtils::Dot(d, tangent);
			float y = MathUtils::Dot(d, bitangent);
			ordered[i] = {f, std::atan2(y, x)};
		}

		std::sort(
		    ordered.begin(),
		    ordered.end(),
		    [](const FaceAngle& a, const FaceAngle& b)
		    {
			    return a.angle < b.angle;
		    });

		uint32_t a = (uint32_t) ordered[0].faceIndex;
		uint32_t b = (uint32_t) ordered[1].faceIndex;
		uint32_t c = (uint32_t) ordered[2].faceIndex;
		uint32_t d = (uint32_t) ordered[3].faceIndex;
		uint32_t e = (uint32_t) ordered[4].faceIndex;

		// Triangulate pentagon (a,b,c,d,e) as fan around a
		outIndices.push_back(a);
		outIndices.push_back(b);
		outIndices.push_back(c);

		outIndices.push_back(a);
		outIndices.push_back(c);
		outIndices.push_back(d);

		outIndices.push_back(a);
		outIndices.push_back(d);
		outIndices.push_back(e);
	}
}
