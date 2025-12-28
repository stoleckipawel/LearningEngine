
#include "PCH.h"
#include "PrimitiveFactory.h"

#include "Primitive.h"

#include <cmath>
#include <random>

#include "Primitives/Basic/PrimitiveBox.h"
#include "Primitives/Basic/PrimitiveCapsule.h"
#include "Primitives/Basic/PrimitiveCone.h"
#include "Primitives/Basic/PrimitiveCylinder.h"
#include "Primitives/Basic/PrimitiveDisk.h"
#include "Primitives/Basic/PrimitiveHemisphere.h"
#include "Primitives/Basic/PrimitivePlane.h"
#include "Primitives/Basic/PrimitivePyramid.h"
#include "Primitives/Basic/PrimitiveSphere.h"
#include "Primitives/Basic/PrimitiveTorus.h"

#include "Primitives/Polyhedra/PrimitiveDodecahedron.h"
#include "Primitives/Polyhedra/PrimitiveIcosahedron.h"
#include "Primitives/Polyhedra/PrimitiveIcosphere.h"
#include "Primitives/Polyhedra/PrimitiveOctahedron.h"
#include "Primitives/Polyhedra/PrimitiveTetrahedron.h"

void PrimitiveFactory::AppendShape(
    PrimitiveFactory::Shape shape,
    const DirectX::XMFLOAT3& translation,
    const DirectX::XMFLOAT3& rotation,
    const DirectX::XMFLOAT3& scale)
{
	switch (shape)
	{
		case Shape::Box:
			m_primitives.push_back(std::make_unique<PrimitiveBox>(translation, rotation, scale));
			break;
		case Shape::Plane:
			m_primitives.push_back(std::make_unique<PrimitivePlane>(translation, rotation, scale));
			break;
		case Shape::Sphere:
			m_primitives.push_back(std::make_unique<PrimitiveSphere>(translation, rotation, scale));
			break;
		case Shape::Cone:
			m_primitives.push_back(std::make_unique<PrimitiveCone>(translation, rotation, scale));
			break;
		case Shape::Cylinder:
			m_primitives.push_back(std::make_unique<PrimitiveCylinder>(translation, rotation, scale));
			break;
		case Shape::Torus:
			m_primitives.push_back(std::make_unique<PrimitiveTorus>(translation, rotation, scale));
			break;
		case Shape::Capsule:
			m_primitives.push_back(std::make_unique<PrimitiveCapsule>(translation, rotation, scale));
			break;
		case Shape::Hemisphere:
			m_primitives.push_back(std::make_unique<PrimitiveHemisphere>(translation, rotation, scale));
			break;
		case Shape::Pyramid:
			m_primitives.push_back(std::make_unique<PrimitivePyramid>(translation, rotation, scale));
			break;
		case Shape::Disk:
			m_primitives.push_back(std::make_unique<PrimitiveDisk>(translation, rotation, scale));
			break;
		case Shape::Octahedron:
			m_primitives.push_back(std::make_unique<PrimitiveOctahedron>(translation, rotation, scale));
			break;
		case Shape::Tetrahedron:
			m_primitives.push_back(std::make_unique<PrimitiveTetrahedron>(translation, rotation, scale));
			break;
		case Shape::Icosahedron:
			m_primitives.push_back(std::make_unique<PrimitiveIcosahedron>(translation, rotation, scale));
			break;
		case Shape::Dodecahedron:
			m_primitives.push_back(std::make_unique<PrimitiveDodecahedron>(translation, rotation, scale));
			break;
		case Shape::Icosphere:
			m_primitives.push_back(std::make_unique<PrimitiveIcosphere>(translation, rotation, scale));
			break;
		default:
			break;
	}
}

void PrimitiveFactory::AppendRandomShapes(
    std::uint32_t count,
    const DirectX::XMFLOAT3& center,
    const DirectX::XMFLOAT3& extents,
    std::uint32_t seed)
{
	if (count == 0)
		return;

	m_primitives.reserve(m_primitives.size() + static_cast<size_t>(count));

	const DirectX::XMFLOAT3 e{std::fabs(extents.x), std::fabs(extents.y), std::fabs(extents.z)};
	const float tx0 = center.x - e.x;
	const float tx1 = center.x + e.x;
	const float ty0 = center.y - e.y;
	const float ty1 = center.y + e.y;
	const float tz0 = center.z - e.z;
	const float tz1 = center.z + e.z;

	std::mt19937 rng;
	if (seed == 0)
	{
		std::random_device rd;
		rng.seed(rd());
	}
	else
	{
		rng.seed(seed);
	}

	std::uniform_real_distribution<float> distTx(tx0, tx1);
	std::uniform_real_distribution<float> distTy(ty0, ty1);
	std::uniform_real_distribution<float> distTz(tz0, tz1);
	std::uniform_real_distribution<float> distRot(-DirectX::XM_PI, DirectX::XM_PI);
	std::uniform_real_distribution<float> distScale(0.25f, 1.5f);

	static constexpr Shape kSpawnableShapes[] = {
	    Shape::Box,
	    Shape::Sphere,
	    Shape::Cone,
	    Shape::Cylinder,
	    Shape::Torus,
	    Shape::Capsule,
	    Shape::Hemisphere,
	    Shape::Pyramid,
	    Shape::Disk,
	    Shape::Octahedron,
	    Shape::Tetrahedron,
	    Shape::Icosahedron,
	    Shape::Dodecahedron,
	    Shape::Icosphere,
	};
	std::uniform_int_distribution<size_t> distShape(0, (sizeof(kSpawnableShapes) / sizeof(kSpawnableShapes[0])) - 1);

	for (std::uint32_t i = 0; i < count; ++i)
	{
		const DirectX::XMFLOAT3 t{distTx(rng), distTy(rng), distTz(rng)};
		const DirectX::XMFLOAT3 r{0.0f, 0.0f, 0.0f};
		const DirectX::XMFLOAT3 s{1.0f, 1.0f, 1.0f};
		AppendShape(kSpawnableShapes[distShape(rng)], t, r, s);
	}
}

const std::vector<std::unique_ptr<Primitive>>& PrimitiveFactory::GetPrimitives() const
{
	return m_primitives;
}

void PrimitiveFactory::Upload()
{
	for (auto& primitive : m_primitives)
	{
		if (primitive)
		{
			primitive->Upload();
		}
	}
}
