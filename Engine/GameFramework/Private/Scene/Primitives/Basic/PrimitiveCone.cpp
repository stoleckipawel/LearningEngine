#include "PCH.h"
#include "Primitives/Basic/PrimitiveCone.h"

using namespace DirectX;

PrimitiveCone::PrimitiveCone(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveCone::GenerateGeometry(MeshData& outMeshData) const
{
	auto& outVertices = outMeshData.vertices;
	auto& outIndices = outMeshData.indices;

	const int slices = 32;
	outVertices.clear();
	outVertices.reserve(slices + 2);

	// Apex at y=1
	DirectX::XMFLOAT3 apex{0.0f, 1.0f, 0.0f};
	// Apex normal points up; tangent choose X axis
	outVertices.push_back({apex, {0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	// Base ring at y=-1
	for (int i = 0; i < slices; ++i)
	{
		float phi = (float) i / (float) slices * DirectX::XM_2PI;
		float x = cosf(phi);
		float z = sinf(phi);
		DirectX::XMFLOAT3 pos{x, -1.0f, z};
		DirectX::XMFLOAT2 uv{(float) i / (float) slices, 1.0f};
		DirectX::XMFLOAT4 color{fabsf(x), 0.5f, fabsf(z), 1.0f};
		// approximate side normal: (x, radius/height, z) with radius=1, height=2 -> y=0.5
		DirectX::XMFLOAT3 normal{x, 0.5f, z};
		// tangent around circumference
		DirectX::XMFLOAT3 tangent{-z, 0.0f, x};
		outVertices.push_back({pos, uv, color, normal, DirectX::XMFLOAT4{tangent.x, tangent.y, tangent.z, 1.0f}});
	}

	// Base center
	DirectX::XMFLOAT3 baseCenter{0.0f, -1.0f, 0.0f};
	// base center normal points down
	outVertices.push_back({baseCenter, {0.5f, 0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	outIndices.clear();
	outIndices.reserve(slices * 6);

	// Apex is vertex 0, ring starts at 1, base center is last
	uint32_t apex_idx = 0;
	uint32_t baseStart = 1;
	uint32_t baseCenter_idx = (uint32_t) (1 + slices);

	for (int i = 0; i < slices; ++i)
	{
		uint32_t next = (uint32_t) (baseStart + ((i + 1) % slices));
		uint32_t cur = (uint32_t) (baseStart + i);

		// side triangle (cur, next, apex) - use winding consistent with others
		outIndices.push_back(cur);
		outIndices.push_back(next);
		outIndices.push_back(apex_idx);

		// base triangle (baseCenter, next, cur)
		outIndices.push_back(baseCenter_idx);
		outIndices.push_back(next);
		outIndices.push_back(cur);
	}
}
