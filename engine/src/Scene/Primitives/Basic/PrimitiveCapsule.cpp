#include "PCH.h"
#include "Primitives/Basic/PrimitiveCapsule.h"

#include "MathUtils.h"

PrimitiveCapsule::PrimitiveCapsule(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveCapsule::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	const int lonSegments = 32;
	const int hemiStacks = 8;
	const int cylStacks = 4;
	const float radius = 0.5f;
	const float halfCylinder = 0.5f;

	// Rings: bottom hemisphere (hemiStacks+1) + cylinder (cylStacks) + top hemisphere (hemiStacks)
	// Cylinder rings include the top equator; bottom equator comes from hemisphere.
	const int ringCount = 2 * hemiStacks + cylStacks + 1;

	outVertices.clear();
	outVertices.reserve((size_t) ringCount * (size_t) (lonSegments + 1));

	auto addRing = [&](float y, float r)
	{
		for (int lon = 0; lon <= lonSegments; ++lon)
		{
			float u = (float) lon / (float) lonSegments;
			float phi = u * DirectX::XM_2PI;
			float c = std::cosf(phi);
			float s = std::sinf(phi);

			float x = r * c;
			float z = r * s;
			DirectX::XMFLOAT3 pos{x, y, z};

			DirectX::XMFLOAT3 normal;
			if (y > halfCylinder + 1e-6f)
			{
				DirectX::XMFLOAT3 center{0.0f, halfCylinder, 0.0f};
				normal = MathUtils::Normalize3({pos.x - center.x, pos.y - center.y, pos.z - center.z});
			}
			else if (y < -halfCylinder - 1e-6f)
			{
				DirectX::XMFLOAT3 center{0.0f, -halfCylinder, 0.0f};
				normal = MathUtils::Normalize3({pos.x - center.x, pos.y - center.y, pos.z - center.z});
			}
			else
			{
				normal = MathUtils::Normalize3({pos.x, 0.0f, pos.z});
			}

			DirectX::XMFLOAT2 uv{u, (y + 1.0f) * 0.5f};
			DirectX::XMFLOAT3 tangent3{-s, 0.0f, c};
			tangent3 = MathUtils::Normalize3(tangent3);

			DirectX::XMFLOAT4 color{std::fabs(normal.x), std::fabs(normal.y), std::fabs(normal.z), 1.0f};
			outVertices.push_back({pos, uv, color, normal, {tangent3.x, tangent3.y, tangent3.z, 1.0f}});
		}
	};

	// Bottom tip -> bottom equator
	for (int i = hemiStacks; i >= 0; --i)
	{
		float t = (float) i / (float) hemiStacks;
		float a = t * (DirectX::XM_PI * 0.5f);  // 0..pi/2
		float r = radius * std::cosf(a);
		float y = -halfCylinder - radius * std::sinf(a);
		addRing(y, r);
	}

	// Cylinder rings (exclude bottom equator, include top equator)
	for (int j = 1; j <= cylStacks; ++j)
	{
		float t = (float) j / (float) cylStacks;
		float y = -halfCylinder + (2.0f * halfCylinder) * t;
		addRing(y, radius);
	}

	// Top hemisphere (exclude equator)
	for (int i = 1; i <= hemiStacks; ++i)
	{
		float t = (float) i / (float) hemiStacks;
		float a = t * (DirectX::XM_PI * 0.5f);
		float r = radius * std::cosf(a);
		float y = halfCylinder + radius * std::sinf(a);
		addRing(y, r);
	}
}

void PrimitiveCapsule::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	const int lonSegments = 32;
	const int hemiStacks = 8;
	const int cylStacks = 4;
	const int ringCount = 2 * hemiStacks + cylStacks + 1;

	outIndices.clear();
	outIndices.reserve((size_t) (ringCount - 1) * (size_t) lonSegments * 6);

	for (int ring = 0; ring < ringCount - 1; ++ring)
	{
		for (int lon = 0; lon < lonSegments; ++lon)
		{
			DWORD first = (DWORD) (ring * (lonSegments + 1) + lon);
			DWORD second = (DWORD) ((ring + 1) * (lonSegments + 1) + lon);

			outIndices.push_back(first);
			outIndices.push_back(second);
			outIndices.push_back(first + 1);

			outIndices.push_back(second);
			outIndices.push_back(second + 1);
			outIndices.push_back(first + 1);
		}
	}
}
