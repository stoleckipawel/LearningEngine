
#pragma once

#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Primitive;

// Factory for creating and managing renderable primitives; append, upload, and update them.
class PrimitiveFactory
{
  public:
	// Construct a new PrimitiveFactory object.
	PrimitiveFactory() = default;
	~PrimitiveFactory() = default;

	// Generic shape enum
	enum class Shape
	{
		Box,
		Plane,
		Sphere,
		Cone,
		Cylinder,
		Torus,
		Capsule,
		Hemisphere,
		Pyramid,
		Disk,
		Octahedron,
		Tetrahedron,
		Icosahedron,
		Dodecahedron,
		Icosphere,
	};

	// Append a primitive by enum shape
	void AppendShape(
	    Shape shape,
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Append N spheres with random positions inside an AABB defined by (center +/- extents).
	// Rotation/scale use the PrimitiveSphere defaults.
	// If seed == 0, a non-deterministic seed is used.
	void AppendRandomSpheres(
	    std::uint32_t count,
	    const DirectX::XMFLOAT3& center,
	    const DirectX::XMFLOAT3& extents,
	    std::uint32_t seed = 0);

	// Upload all primitives geometry to the GPU.
	void Upload();

	// Get const reference to the vector of all managed primitives.
	const std::vector<std::unique_ptr<Primitive>>& GetPrimitives() const;
  private:
	// Storage for all managed primitives.
	std::vector<std::unique_ptr<Primitive>> m_primitives;
};
