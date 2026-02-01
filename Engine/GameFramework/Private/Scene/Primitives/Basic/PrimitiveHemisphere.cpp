#include "PCH.h"
#include "Primitives/Basic/PrimitiveHemisphere.h"

#include "MathUtils.h"

using namespace DirectX;

PrimitiveHemisphere::PrimitiveHemisphere(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveHemisphere::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	const int latSegments = 8;
	const int lonSegments = 16;

	outVertices.clear();
	// Curved surface vertices + cap vertices
	outVertices.reserve((size_t) (latSegments + 1) * (lonSegments + 1) + (size_t) (lonSegments + 2));

	// Curved surface: theta 0..pi/2
	for (int lat = 0; lat <= latSegments; ++lat)
	{
		float tLat = (float) lat / (float) latSegments;
		float theta = tLat * (DirectX::XM_PI * 0.5f);
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int lon = 0; lon <= lonSegments; ++lon)
		{
			float tLon = (float) lon / (float) lonSegments;
			float phi = tLon * DirectX::XM_2PI;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			float x = sinTheta * cosPhi;
			float y = cosTheta;
			float z = sinTheta * sinPhi;

			DirectX::XMFLOAT3 pos{x, y, z};
			DirectX::XMFLOAT3 normal = MathUtils::Normalize3(pos);
			DirectX::XMFLOAT2 uv{tLon, tLat};

			DirectX::XMFLOAT3 tangent3{-sinPhi, 0.0f, cosPhi};
			tangent3 = MathUtils::Normalize3(tangent3);

			DirectX::XMFLOAT4 color{fabsf(normal.x), fabsf(normal.y), fabsf(normal.z), 1.0f};
			outVertices.push_back({pos, uv, color, normal, {tangent3.x, tangent3.y, tangent3.z, 1.0f}});
		}
	}

	// Base cap (y = 0), outward is -Y
	const DWORD capCenterIndex = (DWORD) outVertices.size();
	outVertices.push_back({{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f}, {0.7f, 0.7f, 0.7f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	for (int lon = 0; lon <= lonSegments; ++lon)
	{
		float tLon = (float) lon / (float) lonSegments;
		float phi = tLon * DirectX::XM_2PI;
		float x = cosf(phi);
		float z = sinf(phi);

		DirectX::XMFLOAT3 pos{x, 0.0f, z};
		DirectX::XMFLOAT2 uv{0.5f + 0.5f * x, 0.5f - 0.5f * z};
		DirectX::XMFLOAT4 color{fabsf(x), 0.3f, fabsf(z), 1.0f};
		outVertices.push_back({pos, uv, color, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	}

	(void) capCenterIndex;
}

void PrimitiveHemisphere::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	const int latSegments = 8;
	const int lonSegments = 16;

	outIndices.clear();
	// Curved surface
	outIndices.reserve((size_t) latSegments * lonSegments * 6 + (size_t) lonSegments * 3);

	for (int lat = 0; lat < latSegments; ++lat)
	{
		for (int lon = 0; lon < lonSegments; ++lon)
		{
			DWORD first = (DWORD) (lat * (lonSegments + 1) + lon);
			DWORD second = (DWORD) ((lat + 1) * (lonSegments + 1) + lon);

			outIndices.push_back(first);
			outIndices.push_back(second);
			outIndices.push_back(first + 1);

			outIndices.push_back(second);
			outIndices.push_back(second + 1);
			outIndices.push_back(first + 1);
		}
	}

	// Cap fan (faces downward): center, next, current
	const DWORD curvedVertexCount = (DWORD) ((latSegments + 1) * (lonSegments + 1));
	const DWORD capCenterIndex = curvedVertexCount;
	const DWORD capRingStart = capCenterIndex + 1;

	for (int lon = 0; lon < lonSegments; ++lon)
	{
		outIndices.push_back(capCenterIndex);
		outIndices.push_back((DWORD) (capRingStart + lon + 1));
		outIndices.push_back((DWORD) (capRingStart + lon));
	}
}
