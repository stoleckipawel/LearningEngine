#include "PCH.h"
#include "MeshFactory.h"
#include "Scene/Mesh.h"

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

#include <cmath>
#include <random>

void MeshFactory::AppendShape(
    Shape shape,
    const DirectX::XMFLOAT3& translation,
    const DirectX::XMFLOAT3& rotation,
    const DirectX::XMFLOAT3& scale)
{
	std::unique_ptr<Mesh> mesh;

	switch (shape)
	{
		case Shape::Box:
			mesh = std::make_unique<PrimitiveBox>(translation, rotation, scale);
			break;
		case Shape::Plane:
			mesh = std::make_unique<PrimitivePlane>(translation, rotation, scale);
			break;
		case Shape::Sphere:
			mesh = std::make_unique<PrimitiveSphere>(translation, rotation, scale);
			break;
		case Shape::Cone:
			mesh = std::make_unique<PrimitiveCone>(translation, rotation, scale);
			break;
		case Shape::Cylinder:
			mesh = std::make_unique<PrimitiveCylinder>(translation, rotation, scale);
			break;
		case Shape::Torus:
			mesh = std::make_unique<PrimitiveTorus>(translation, rotation, scale);
			break;
		case Shape::Capsule:
			mesh = std::make_unique<PrimitiveCapsule>(translation, rotation, scale);
			break;
		case Shape::Hemisphere:
			mesh = std::make_unique<PrimitiveHemisphere>(translation, rotation, scale);
			break;
		case Shape::Pyramid:
			mesh = std::make_unique<PrimitivePyramid>(translation, rotation, scale);
			break;
		case Shape::Disk:
			mesh = std::make_unique<PrimitiveDisk>(translation, rotation, scale);
			break;
		case Shape::Octahedron:
			mesh = std::make_unique<PrimitiveOctahedron>(translation, rotation, scale);
			break;
		case Shape::Tetrahedron:
			mesh = std::make_unique<PrimitiveTetrahedron>(translation, rotation, scale);
			break;
		case Shape::Icosahedron:
			mesh = std::make_unique<PrimitiveIcosahedron>(translation, rotation, scale);
			break;
		case Shape::Dodecahedron:
			mesh = std::make_unique<PrimitiveDodecahedron>(translation, rotation, scale);
			break;
		case Shape::Icosphere:
			mesh = std::make_unique<PrimitiveIcosphere>(translation, rotation, scale);
			break;
	}

	if (mesh)
	{
		m_meshes.push_back(std::move(mesh));
	}
}

void MeshFactory::AppendShapes(
    Shape shape,
    uint32 count,
    const DirectX::XMFLOAT3& center,
    const DirectX::XMFLOAT3& extents,
    uint32 seed)
{
	if (count == 0)
		return;

	m_meshes.reserve(m_meshes.size() + count);

	const DirectX::XMFLOAT3 e{std::fabs(extents.x), std::fabs(extents.y), std::fabs(extents.z)};
	const float tx0 = center.x - e.x;
	const float tx1 = center.x + e.x;
	const float ty0 = center.y - e.y;
	const float ty1 = center.y + e.y;
	const float tz0 = center.z - e.z;
	const float tz1 = center.z + e.z;

	std::mt19937 rng(seed != 0 ? seed : std::random_device{}());
	std::uniform_real_distribution<float> distX(tx0, tx1);
	std::uniform_real_distribution<float> distY(ty0, ty1);
	std::uniform_real_distribution<float> distZ(tz0, tz1);

	for (uint32 i = 0; i < count; ++i)
	{
		const DirectX::XMFLOAT3 t{distX(rng), distY(rng), distZ(rng)};
		AppendShape(shape, t, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	}
}

void MeshFactory::Clear() noexcept
{
	m_meshes.clear();
}

void MeshFactory::Rebuild(
    Shape shape,
    uint32 count,
    const DirectX::XMFLOAT3& center,
    const DirectX::XMFLOAT3& extents,
    uint32 seed)
{
	Clear();
	AppendShapes(shape, count, center, extents, seed);
}
