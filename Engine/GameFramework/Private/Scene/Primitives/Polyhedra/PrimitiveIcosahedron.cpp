#include "PCH.h"
#include "Primitives/Polyhedra/PrimitiveIcosahedron.h"

#include "MathUtils.h"

#include <array>

using namespace DirectX;

PrimitiveIcosahedron::PrimitiveIcosahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale) :
    Mesh(translation, rotation, scale)
{
}

void PrimitiveIcosahedron::GenerateVertices(std::vector<Vertex>& outVertices) const
{
	const float phi = (1.0f + sqrtf(5.0f)) * 0.5f;

	std::array<DirectX::XMFLOAT3, 12> base = {
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

	outVertices.clear();
	outVertices.reserve(base.size());

	for (auto& p : base)
	{
		DirectX::XMFLOAT3 n = MathUtils::Normalize3(p);
		DirectX::XMFLOAT2 uv = MathUtils::SphericalUV(n);

		float phiAng = std::atan2(n.z, n.x);
		DirectX::XMFLOAT3 tangent{-sinf(phiAng), 0.0f, cosf(phiAng)};
		tangent = MathUtils::Normalize3(tangent);

		DirectX::XMFLOAT4 color{fabsf(n.x), fabsf(n.y), fabsf(n.z), 1.0f};
		outVertices.push_back({n, uv, color, n, {tangent.x, tangent.y, tangent.z, 1.0f}});
	}
}

void PrimitiveIcosahedron::GenerateIndices(std::vector<DWORD>& outIndices) const
{
	outIndices = {
	    0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,  11, 10, 2,  10, 7, 6, 7, 1, 8,
	    3, 9,  4, 3, 4, 2, 3, 2, 6, 3, 6, 8,  3, 8,  9,  4, 9, 5, 2, 4,  11, 6,  2,  10, 8,  6, 7, 9, 8, 1,
	};
}
