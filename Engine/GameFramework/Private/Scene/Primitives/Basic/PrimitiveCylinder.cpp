#include "PCH.h"
#include "Primitives/Basic/PrimitiveCylinder.h"

using namespace DirectX;

PrimitiveCylinder::PrimitiveCylinder(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveCylinder::GenerateGeometry(MeshData& outMeshData) const
{
	auto& outVertices = outMeshData.vertices;
	auto& outIndices = outMeshData.indices;

	const int slices = 32;
	outVertices.clear();
	outVertices.reserve((slices + 1) * 2 + 2);

	// Bottom ring y = -1
	for (int i = 0; i < slices; ++i)
	{
		float phi = (float) i / (float) slices * DirectX::XM_2PI;
		float x = cosf(phi);
		float z = sinf(phi);
		// side vertex bottom ring: normal points outward, tangent along circumference
		DirectX::XMFLOAT3 normal{x, 0.0f, z};
		DirectX::XMFLOAT3 tangent{-z, 0.0f, x};
		outVertices.push_back(
		    {{x, -1.0f, z},
		     {(float) i / slices, 1.0f},
		     {fabsf(x), 0.3f, fabsf(z), 1.0f},
		     normal,
		     DirectX::XMFLOAT4{tangent.x, tangent.y, tangent.z, 1.0f}});
	}

	// Top ring y = 1
	for (int i = 0; i < slices; ++i)
	{
		float phi = (float) i / (float) slices * DirectX::XM_2PI;
		float x = cosf(phi);
		float z = sinf(phi);
		DirectX::XMFLOAT3 normal{x, 0.0f, z};
		DirectX::XMFLOAT3 tangent{-z, 0.0f, x};
		outVertices.push_back(
		    {{x, 1.0f, z},
		     {(float) i / slices, 0.0f},
		     {0.3f, fabsf(x), fabsf(z), 1.0f},
		     normal,
		     DirectX::XMFLOAT4{tangent.x, tangent.y, tangent.z, 1.0f}});
	}

	// Bottom center
	outVertices.push_back({{0.0f, -1.0f, 0.0f}, {0.5f, 1.0f}, {0.6f, 0.6f, 0.6f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});
	// Top center
	outVertices.push_back({{0.0f, 1.0f, 0.0f}, {0.5f, 0.0f}, {0.9f, 0.9f, 0.9f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}});

	outIndices.clear();
	outIndices.reserve(slices * 12);

	uint32_t bottomStart = 0;
	uint32_t topStart = (uint32_t) slices;
	uint32_t bottomCenter = (uint32_t) (slices * 2);
	uint32_t topCenter = (uint32_t) (slices * 2 + 1);

	// Side quads (two triangles each)
	for (int i = 0; i < slices; ++i)
	{
		uint32_t next = (uint32_t) ((i + 1) % slices);
		uint32_t b0 = bottomStart + i;
		uint32_t b1 = bottomStart + next;
		uint32_t t0 = topStart + i;
		uint32_t t1 = topStart + next;

		outIndices.push_back(b0);
		outIndices.push_back(t1);
		outIndices.push_back(t0);

		outIndices.push_back(b0);
		outIndices.push_back(b1);
		outIndices.push_back(t1);
	}

	// Bottom cap
	for (int i = 0; i < slices; ++i)
	{
		uint32_t next = (uint32_t) ((i + 1) % slices);
		outIndices.push_back(bottomCenter);
		outIndices.push_back(bottomStart + next);
		outIndices.push_back(bottomStart + i);
	}

	// Top cap
	for (int i = 0; i < slices; ++i)
	{
		uint32_t next = (uint32_t) ((i + 1) % slices);
		outIndices.push_back(topCenter);
		outIndices.push_back(topStart + i);
		outIndices.push_back(topStart + next);
	}
}
