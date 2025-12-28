#pragma once

#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Mesh;

// Factory for creating and managing renderable meshes; append, upload, and update them.
// Today this primarily creates built-in primitive meshes (box, sphere, etc.).
class MeshFactory
{
  public:
	MeshFactory() = default;
	~MeshFactory() = default;

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

	// Append a mesh by enum shape
	void AppendShape(
	    Shape shape,
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Append N random shapes with random positions inside an AABB defined by (center +/- extents).
	// Rotation/scale use the Mesh defaults (rotation = 0, scale = 1).
	// If seed == 0, a non-deterministic seed is used.
	void AppendRandomShapes(std::uint32_t count, const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents, std::uint32_t seed = 0);

	// Upload all meshes geometry to the GPU.
	void Upload();

	// Get const reference to the vector of all managed meshes.
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const;

  private:
	// Storage for all managed meshes.
	std::vector<std::unique_ptr<Mesh>> m_meshes;
};
