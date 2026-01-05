// ============================================================================
// MeshFactory.h
// ----------------------------------------------------------------------------
// Factory for creating and managing renderable mesh primitives.
//
// USAGE:
//   MeshFactory factory;
//   factory.Rebuild(MeshFactory::Shape::Sphere, 100, {0,0,0}, {10,10,10}, 42);
//   factory.Upload();  // Send to GPU
//   for (const auto& mesh : factory.GetMeshes()) { ... }
//
// DESIGN:
//   - Supports various primitive shapes (box, sphere, torus, etc.)
//   - Random placement within AABB via seed for reproducibility
//   - Owns mesh instances; caller gets const reference
//
// NOTES:
//   - Call Upload() after Rebuild()/AppendShape() to transfer to GPU
//   - Clear() releases all meshes
// ============================================================================

#pragma once

#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Mesh;

class MeshFactory
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	MeshFactory() = default;
	~MeshFactory() = default;

	// ========================================================================
	// Shape Enumeration
	// ========================================================================

	/// Available primitive shapes for mesh generation.
	enum class Shape
	{
		Box,          ///< Axis-aligned box (cube)
		Plane,        ///< Flat quad
		Sphere,       ///< UV sphere
		Cone,         ///< Cone with circular base
		Cylinder,     ///< Cylinder with circular caps
		Torus,        ///< Donut shape
		Capsule,      ///< Cylinder with hemispherical caps
		Hemisphere,   ///< Half sphere
		Pyramid,      ///< Four-sided pyramid
		Disk,         ///< Flat circular disk
		Octahedron,   ///< 8-faced polyhedron
		Tetrahedron,  ///< 4-faced polyhedron
		Icosahedron,  ///< 20-faced polyhedron
		Dodecahedron, ///< 12-faced polyhedron
		Icosphere,    ///< Subdivided icosahedron (uniform triangles)
	};

	// ========================================================================
	// Factory Methods
	// ========================================================================

	/// Clears all meshes and spawns 'count' instances of 'shape' randomly
	/// within an AABB defined by 'center' and 'extents'. Uploads to GPU.
	void Rebuild(
	    Shape shape,
	    std::uint32_t count,
	    const DirectX::XMFLOAT3& center,
	    const DirectX::XMFLOAT3& extents,
	    std::uint32_t seed = 0);

	/// Appends a single mesh by shape with explicit TRS transform.
	void AppendShape(
	    Shape shape,
	    const DirectX::XMFLOAT3& translation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& rotation = {0.0f, 0.0f, 0.0f},
	    const DirectX::XMFLOAT3& scale = {1.0f, 1.0f, 1.0f});

	/// Appends N instances of a single shape type randomly within an AABB.
	void AppendShapes(
	    Shape shape,
	    std::uint32_t count,
	    const DirectX::XMFLOAT3& center,
	    const DirectX::XMFLOAT3& extents,
	    std::uint32_t seed = 0);

	// ========================================================================
	// Management
	// ========================================================================

	/// Releases all existing meshes.
	void Clear() noexcept;

	/// Uploads all meshes to the GPU.
	void Upload();

	// ========================================================================
	// Accessors
	// ========================================================================

	/// Returns read-only access to all created meshes.
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const;

  private:
	// ------------------------------------------------------------------------
	// Owned Meshes
	// ------------------------------------------------------------------------

	std::vector<std::unique_ptr<Mesh>> m_meshes;  ///< Created mesh instances
};
