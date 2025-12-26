
#pragma once

#include <memory>
#include <vector>
#include "Primitive.h"
#include "Primitives/Basic/PrimitiveBox.h"
#include "Primitives/Basic/PrimitivePlane.h"
#include "Primitives/Basic/PrimitiveSphere.h"
#include "Primitives/Basic/PrimitiveCone.h"
#include "Primitives/Basic/PrimitiveCylinder.h"
#include "Primitives/Basic/PrimitiveTorus.h"
#include "Primitives/Polyhedra/PrimitiveOctahedron.h"
#include "Primitives/Polyhedra/PrimitiveTetrahedron.h"

// Factory for creating and managing renderable primitives; append, upload, and update them.
class PrimitiveFactory
{
  public:
	// Construct a new PrimitiveFactory object.
	PrimitiveFactory() = default;
	~PrimitiveFactory() = default;

	void AppendBox(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	void AppendPlane(
	    const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	void AppendCone(
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	void AppendCylinder(
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	void AppendTorus(
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	void AppendOctahedron(
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	void AppendTetrahedron(
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Generic shape enum
	enum class Shape
	{
		Box,
		Plane,
		Sphere,
		Cone,
		Cylinder,
		Torus,
		Octahedron,
		Tetrahedron,
	};

	// Append a primitive by enum shape
	void AppendShape(
		Shape shape,
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Upload all primitives geometry to the GPU.
	void Upload();

	// Append a new sphere primitive to the factory.
	void AppendSphere(
		const XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
		const XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Get const reference to the vector of all managed primitives.
	const std::vector<std::unique_ptr<Primitive>>& GetPrimitives() const;

	// Get reference to the first primitive (useful for PSO setup).
	Primitive& GetFirstPrimitive() const;

  private:
	// Storage for all managed primitives.
	std::vector<std::unique_ptr<Primitive>> m_primitives;
};
