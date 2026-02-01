#include "PCH.h"
#include "Primitives/Basic/PrimitiveCylinder.h"

using namespace DirectX;

PrimitiveCylinder::PrimitiveCylinder(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveCylinder::GenerateVertices(std::vector<Vertex>& outVertices) const
{
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
}

void PrimitiveCylinder::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	const int slices = 32;
	outIndices.clear();
	outIndices.reserve(slices * 12);

	DWORD bottomStart = 0;
	DWORD topStart = (DWORD) slices;
	DWORD bottomCenter = (DWORD) (slices * 2);
	DWORD topCenter = (DWORD) (slices * 2 + 1);

	// Side quads (two triangles each)
	for (int i = 0; i < slices; ++i)
	{
		DWORD next = (DWORD) ((i + 1) % slices);
		DWORD b0 = bottomStart + i;
		DWORD b1 = bottomStart + next;
		DWORD t0 = topStart + i;
		DWORD t1 = topStart + next;

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
		DWORD next = (DWORD) ((i + 1) % slices);
		outIndices.push_back(bottomCenter);
		outIndices.push_back(bottomStart + next);
		outIndices.push_back(bottomStart + i);
	}

	// Top cap
	for (int i = 0; i < slices; ++i)
	{
		DWORD next = (DWORD) ((i + 1) % slices);
		outIndices.push_back(topCenter);
		outIndices.push_back(topStart + i);
		outIndices.push_back(topStart + next);
	}
}
