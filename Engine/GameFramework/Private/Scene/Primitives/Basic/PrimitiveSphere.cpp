#include "PCH.h"
#include "Primitives/Basic/PrimitiveSphere.h"

using namespace DirectX;

PrimitiveSphere::PrimitiveSphere(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveSphere::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	// UV sphere parameterization
	const int latSegments = 16;
	const int lonSegments = 16;
	outVertices.clear();
	outVertices.reserve((latSegments + 1) * (lonSegments + 1));

	for (int lat = 0; lat <= latSegments; ++lat)
	{
		float theta = (float) lat / (float) latSegments * DirectX::XM_PI;  // 0..PI
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int lon = 0; lon <= lonSegments; ++lon)
		{
			float phi = (float) lon / (float) lonSegments * DirectX::XM_2PI;  // 0..2PI
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			float x = sinTheta * cosPhi;
			float y = cosTheta;
			float z = sinTheta * sinPhi;

			DirectX::XMFLOAT3 pos{x, y, z};
			DirectX::XMFLOAT2 uv{(float) lon / (float) lonSegments, (float) lat / (float) latSegments};
			DirectX::XMFLOAT4 color{fabsf(x), fabsf(y), fabsf(z), 1.0f};

			// normal is position on unit sphere
			DirectX::XMFLOAT3 normal = pos;
			// tangent: prefer longitude direction; compute as cross(up, normal)
			DirectX::XMFLOAT3 tangent;
			const DirectX::XMFLOAT3 up{0.0f, 1.0f, 0.0f};
			// cross(up, normal)
			tangent.x = up.y * normal.z - up.z * normal.y;
			tangent.y = up.z * normal.x - up.x * normal.z;
			tangent.z = up.x * normal.y - up.y * normal.x;
			// if near poles, fallback tangent
			float len2 = tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z;
			if (len2 < 1e-6f)
				tangent = DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f};
			else
			{
				float invLen = 1.0f / sqrtf(len2);
				tangent.x *= invLen;
				tangent.y *= invLen;
				tangent.z *= invLen;
			}

			outVertices.push_back({pos, uv, color, normal, {tangent.x, tangent.y, tangent.z, 1.0f}});
		}
	}
}

void PrimitiveSphere::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	const int latSegments = 16;
	const int lonSegments = 16;
	outIndices.clear();
	outIndices.reserve(latSegments * lonSegments * 6);

	for (int lat = 0; lat < latSegments; ++lat)
	{
		for (int lon = 0; lon < lonSegments; ++lon)
		{
			DWORD first = (DWORD) (lat * (lonSegments + 1) + lon);
			DWORD second = (DWORD) ((lat + 1) * (lonSegments + 1) + lon);

			// Two triangles per quad on the sphere surface
			outIndices.push_back(first);
			outIndices.push_back(second);
			outIndices.push_back(first + 1);

			outIndices.push_back(second);
			outIndices.push_back(second + 1);
			outIndices.push_back(first + 1);
		}
	}
}
