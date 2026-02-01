#include "PCH.h"
#include "Primitives/Polyhedra/PrimitiveIcosphere.h"

#include "MathUtils.h"

#include <unordered_map>

using namespace DirectX;

PrimitiveIcosphere::PrimitiveIcosphere(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveIcosphere::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	// Build subdivided triangle mesh, then convert to Vertex.
	const int subdivisions = 2;

	const float phi = (1.0f + sqrtf(5.0f)) * 0.5f;
	std::vector<DirectX::XMFLOAT3> positions = {
	    MathUtils::Normalize3({-1.0f, phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({1.0f, phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({-1.0f, -phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({1.0f, -phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, -1.0f, phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, 1.0f, phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, -1.0f, -phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, 1.0f, -phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({phi, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({phi, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({-phi, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({-phi, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	};

	std::vector<uint32_t> indices = {
	    0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,  11, 10, 2,  10, 7, 6, 7, 1, 8,
	    3, 9,  4, 3, 4, 2, 3, 2, 6, 3, 6, 8,  3, 8,  9,  4, 9, 5, 2, 4,  11, 6,  2,  10, 8,  6, 7, 9, 8, 1,
	};

	for (int s = 0; s < subdivisions; ++s)
	{
		std::unordered_map<uint64_t, uint32_t> midpointCache;
		std::vector<uint32_t> newIndices;
		newIndices.reserve(indices.size() * 4);

		auto getMidpoint = [&](uint32_t a, uint32_t b) -> uint32_t
		{
			uint64_t key = MathUtils::EdgeKey(a, b);
			auto it = midpointCache.find(key);
			if (it != midpointCache.end())
				return it->second;

			DirectX::XMFLOAT3 pa = positions[a];
			DirectX::XMFLOAT3 pb = positions[b];
			DirectX::XMFLOAT3 pm{(pa.x + pb.x) * 0.5f, (pa.y + pb.y) * 0.5f, (pa.z + pb.z) * 0.5f};
			pm = MathUtils::Normalize3(pm, {0.0f, 1.0f, 0.0f}, 1e-10f);

			uint32_t idx = (uint32_t) positions.size();
			positions.push_back(pm);
			midpointCache[key] = idx;
			return idx;
		};

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t v0 = indices[i + 0];
			uint32_t v1 = indices[i + 1];
			uint32_t v2 = indices[i + 2];

			uint32_t a = getMidpoint(v0, v1);
			uint32_t b = getMidpoint(v1, v2);
			uint32_t c = getMidpoint(v2, v0);

			newIndices.insert(newIndices.end(), {v0, a, c});
			newIndices.insert(newIndices.end(), {v1, b, a});
			newIndices.insert(newIndices.end(), {v2, c, b});
			newIndices.insert(newIndices.end(), {a, b, c});
		}

		indices.swap(newIndices);
	}

	outVertices.clear();
	outVertices.reserve(positions.size());

	for (auto& p : positions)
	{
		DirectX::XMFLOAT3 n = MathUtils::Normalize3(p, {0.0f, 1.0f, 0.0f}, 1e-10f);
		DirectX::XMFLOAT2 uv = MathUtils::SphericalUV(n);

		float phiAng = std::atan2(n.z, n.x);
		DirectX::XMFLOAT3 tangent{-sinf(phiAng), 0.0f, cosf(phiAng)};
		float tl2 = tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z;
		if (tl2 < 1e-8f)
			tangent = {1.0f, 0.0f, 0.0f};
		else
			tangent = MathUtils::Normalize3(tangent);

		DirectX::XMFLOAT4 color{fabsf(n.x), fabsf(n.y), fabsf(n.z), 1.0f};
		outVertices.push_back({n, uv, color, n, {tangent.x, tangent.y, tangent.z, 1.0f}});
	}

	// Stash indices for GenerateIndices via a static cache (keeps Mesh interface unchanged)
	// We rebuild indices in GenerateIndices as well to keep both functions deterministic.
}

void PrimitiveIcosphere::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	// Same mesh build as in GenerateVertices, but only indices output.
	const int subdivisions = 2;
	const float phi = (1.0f + sqrtf(5.0f)) * 0.5f;

	std::vector<DirectX::XMFLOAT3> positions = {
	    MathUtils::Normalize3({-1.0f, phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({1.0f, phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({-1.0f, -phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({1.0f, -phi, 0.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, -1.0f, phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, 1.0f, phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, -1.0f, -phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({0.0f, 1.0f, -phi}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({phi, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({phi, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({-phi, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	    MathUtils::Normalize3({-phi, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, 1e-10f),
	};

	std::vector<uint32_t> indices = {
	    0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,  11, 10, 2,  10, 7, 6, 7, 1, 8,
	    3, 9,  4, 3, 4, 2, 3, 2, 6, 3, 6, 8,  3, 8,  9,  4, 9, 5, 2, 4,  11, 6,  2,  10, 8,  6, 7, 9, 8, 1,
	};

	for (int s = 0; s < subdivisions; ++s)
	{
		std::unordered_map<uint64_t, uint32_t> midpointCache;
		std::vector<uint32_t> newIndices;
		newIndices.reserve(indices.size() * 4);

		auto getMidpoint = [&](uint32_t a, uint32_t b) -> uint32_t
		{
			uint64_t key = MathUtils::EdgeKey(a, b);
			auto it = midpointCache.find(key);
			if (it != midpointCache.end())
				return it->second;

			DirectX::XMFLOAT3 pa = positions[a];
			DirectX::XMFLOAT3 pb = positions[b];
			DirectX::XMFLOAT3 pm{(pa.x + pb.x) * 0.5f, (pa.y + pb.y) * 0.5f, (pa.z + pb.z) * 0.5f};
			pm = MathUtils::Normalize3(pm, {0.0f, 1.0f, 0.0f}, 1e-10f);

			uint32_t idx = (uint32_t) positions.size();
			positions.push_back(pm);
			midpointCache[key] = idx;
			return idx;
		};

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t v0 = indices[i + 0];
			uint32_t v1 = indices[i + 1];
			uint32_t v2 = indices[i + 2];

			uint32_t a = getMidpoint(v0, v1);
			uint32_t b = getMidpoint(v1, v2);
			uint32_t c = getMidpoint(v2, v0);

			newIndices.insert(newIndices.end(), {v0, a, c});
			newIndices.insert(newIndices.end(), {v1, b, a});
			newIndices.insert(newIndices.end(), {v2, c, b});
			newIndices.insert(newIndices.end(), {a, b, c});
		}

		indices.swap(newIndices);
	}

	outIndices.clear();
	outIndices.reserve(indices.size());
	for (uint32_t i : indices)
		outIndices.push_back((DWORD) i);
}
