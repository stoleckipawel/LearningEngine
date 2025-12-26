
#include "PCH.h"
#include "PrimitiveFactory.h"

void PrimitiveFactory::AppendBox(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto box = std::make_unique<PrimitiveBox>(translation, rotation, scale);
	m_primitives.push_back(std::move(box));
}

void PrimitiveFactory::AppendPlane(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto plane = std::make_unique<PrimitivePlane>(translation, rotation, scale);
	m_primitives.push_back(std::move(plane));
}

void PrimitiveFactory::AppendSphere(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto sphere = std::make_unique<PrimitiveSphere>(translation, rotation, scale);
	m_primitives.push_back(std::move(sphere));
}

void PrimitiveFactory::AppendCone(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto cone = std::make_unique<PrimitiveCone>(translation, rotation, scale);
	m_primitives.push_back(std::move(cone));
}

void PrimitiveFactory::AppendCylinder(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto cylinder = std::make_unique<PrimitiveCylinder>(translation, rotation, scale);
	m_primitives.push_back(std::move(cylinder));
}

void PrimitiveFactory::AppendTorus(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto torus = std::make_unique<PrimitiveTorus>(translation, rotation, scale);
	m_primitives.push_back(std::move(torus));
}

void PrimitiveFactory::AppendOctahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto oct = std::make_unique<PrimitiveOctahedron>(translation, rotation, scale);
	m_primitives.push_back(std::move(oct));
}

void PrimitiveFactory::AppendTetrahedron(const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	auto tet = std::make_unique<PrimitiveTetrahedron>(translation, rotation, scale);
	m_primitives.push_back(std::move(tet));
}

void PrimitiveFactory::AppendShape(PrimitiveFactory::Shape shape, const XMFLOAT3& translation, const XMFLOAT3& rotation, const XMFLOAT3& scale)
{
	switch (shape)
	{
	case Shape::Box:
		AppendBox(translation, rotation, scale);
		break;
	case Shape::Plane:
		AppendPlane(translation, rotation, scale);
		break;
	case Shape::Sphere:
		AppendSphere(translation, rotation, scale);
		break;
	case Shape::Cone:
		AppendCone(translation, rotation, scale);
		break;
	case Shape::Cylinder:
		AppendCylinder(translation, rotation, scale);
		break;
	case Shape::Torus:
		AppendTorus(translation, rotation, scale);
		break;
	case Shape::Octahedron:
		AppendOctahedron(translation, rotation, scale);
		break;
	case Shape::Tetrahedron:
		AppendTetrahedron(translation, rotation, scale);
		break;
	default:
		break;
	}
}

const std::vector<std::unique_ptr<Primitive>>& PrimitiveFactory::GetPrimitives() const
{
	return m_primitives;
}

Primitive& PrimitiveFactory::GetFirstPrimitive() const
{
	return *m_primitives.front();
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
