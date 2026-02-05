#include "PCH.h"
#include "Primitives/Basic/PrimitiveDisk.h"

using namespace DirectX;

PrimitiveDisk::PrimitiveDisk(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveDisk::GenerateGeometry(MeshData& outMeshData) const
{
	auto& outVertices = outMeshData.vertices;
	auto& outIndices = outMeshData.indices;

	const int slices = 32;
	outVertices.clear();
	outVertices.reserve((size_t) slices + 2);

	// Center
	outVertices.push_back({{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	for (int i = 0; i <= slices; ++i)
	{
		float t = (float) i / (float) slices;
		float phi = t * DirectX::XM_2PI;
		float x = cosf(phi);
		float z = sinf(phi);

		DirectX::XMFLOAT3 pos{x, 0.0f, z};
		DirectX::XMFLOAT2 uv{0.5f + 0.5f * x, 0.5f - 0.5f * z};
		DirectX::XMFLOAT4 color{fabsf(x), 0.2f, fabsf(z), 1.0f};
		DirectX::XMFLOAT3 normal{0.0f, 1.0f, 0.0f};
		DirectX::XMFLOAT4 tangent{1.0f, 0.0f, 0.0f, 1.0f};

		outVertices.push_back({pos, uv, color, normal, tangent});
	}

	outIndices.clear();
	outIndices.reserve((size_t) slices * 3);

	// Triangle fan around center (0), ring starts at 1
	for (int i = 0; i < slices; ++i)
	{
		outIndices.push_back(0);
		outIndices.push_back((uint32_t) (1 + i));
		outIndices.push_back((uint32_t) (1 + i + 1));
	}
}
