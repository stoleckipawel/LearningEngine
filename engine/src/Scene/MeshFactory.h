#pragma once

#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Mesh;

// Factory for creating and managing renderable meshes.
class MeshFactory
{
  public:
	MeshFactory() = default;
	~MeshFactory() = default;


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

	// Clears all meshes and spawns 'count' instances of 'shape' spread randomly
	// around 'center' within 'extents'. Uploads geometry to the GPU.
	void Rebuild(Shape shape, std::uint32_t count, const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents, std::uint32_t seed = 0);

	// Append a single mesh by shape enum.
	void AppendShape(
	    Shape shape,
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	// Append N instances of a single shape type inside an AABB.
	void AppendShapes(
	    Shape shape,
	    std::uint32_t count,
	    const DirectX::XMFLOAT3& center,
	    const DirectX::XMFLOAT3& extents,
	    std::uint32_t seed = 0);

	// Clears all existing meshes.
	void Clear() noexcept;

	// Uploads all meshes to the GPU.
	void Upload();

	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const;

  private:
	std::vector<std::unique_ptr<Mesh>> m_meshes;
};
