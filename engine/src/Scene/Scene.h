// ============================================================================
// Scene.h
// ----------------------------------------------------------------------------
// Manages renderable objects and scene state for the application.
//
// USAGE:
//   GScene.Initialize();
//   GScene.SetPrimitives(MeshFactory::Shape::Sphere, 64);
//   for (const auto& mesh : GScene.GetMeshes()) { ... }
//   GScene.Shutdown();
//
// DESIGN:
//   - PrimitiveConfig struct holds all parameters for procedural generation
//   - Geometry is rebuilt immediately when configuration changes
//   - MeshFactory creates actual geometry based on configuration
//
// NOTES:
//   - Singleton accessed via GScene global reference
//   - Initialize() must be called before use
//   - Meshes are owned by Scene; returned as const references
// ============================================================================

#pragma once

#include "MeshFactory.h"
#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Mesh;

class Scene final
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	/// Returns the singleton Scene instance.
	[[nodiscard]] static Scene& Get() noexcept;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	/// Initializes scene resources and mesh factory. Call once at startup.
	void Initialize();

	/// Releases all scene resources. Safe to call multiple times.
	void Shutdown() noexcept;

	// ========================================================================
	// Configuration
	// ========================================================================

	/// Configuration for procedural primitive spawning.
	struct PrimitiveConfig
	{
		MeshFactory::Shape shape = MeshFactory::Shape::Box;  ///< Primitive type to spawn
		std::uint32_t count = 128;                           ///< Number of primitives
		DirectX::XMFLOAT3 center = {0.0f, 0.0f, 10.0f};      ///< Center of spawn volume
		DirectX::XMFLOAT3 extents = {20.0f, 20.0f, 20.0f};   ///< Half-extents of spawn volume
		std::uint32_t seed = 1337;                           ///< Random seed for positions
	};

	/// Updates primitive configuration and rebuilds geometry immediately.
	void SetPrimitiveConfig(const PrimitiveConfig& config);

	/// Convenience method to update only shape and count (common UI case).
	void SetPrimitives(MeshFactory::Shape shape, std::uint32_t count);

	// ========================================================================
	// Accessors
	// ========================================================================

	/// Returns the current primitive configuration.
	[[nodiscard]] const PrimitiveConfig& GetPrimitiveConfig() const noexcept { return m_primitiveConfig; }

	/// Returns the current primitive shape type.
	[[nodiscard]] MeshFactory::Shape GetCurrentShape() const noexcept { return m_primitiveConfig.shape; }

	/// Returns the current primitive count.
	[[nodiscard]] std::uint32_t GetCurrentCount() const noexcept { return m_primitiveConfig.count; }

	/// Returns read-only access to all meshes for rendering.
	[[nodiscard]] const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const noexcept;

	/// Returns true if the scene has any meshes to render.
	[[nodiscard]] bool HasMeshes() const noexcept;

  private:
	Scene() = default;
	~Scene() noexcept;

	// ------------------------------------------------------------------------
	// Internal Helpers
	// ------------------------------------------------------------------------

	/// Destroys existing meshes and recreates based on m_primitiveConfig.
	void RebuildGeometry();

	// ------------------------------------------------------------------------
	// State
	// ------------------------------------------------------------------------

	PrimitiveConfig m_primitiveConfig;              ///< Current primitive generation settings
	std::unique_ptr<MeshFactory> m_meshFactory;     ///< Factory for creating mesh geometry
	bool m_initialized = false;                     ///< True after Initialize() succeeds
};

/// Global singleton reference for convenient access.
inline Scene& GScene = Scene::Get();
