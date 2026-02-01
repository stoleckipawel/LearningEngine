// ============================================================================
// Scene.h
// ----------------------------------------------------------------------------
// Container for gameplay objects (camera, meshes, etc.).
//
// USAGE:
//   Scene scene;  // Ready to use immediately
//
// DESIGN:
//   - Pure container for logical game objects
//   - No direct GPU/RHI dependencies (decoupled from rendering backend)
//   - Camera and mesh data created in constructor
//   - Scene owns its objects, external systems configure them
//   - GPU resource upload handled externally by MeshFactory::Upload()
//
// ============================================================================

#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"

#include "MeshFactory.h"
#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Mesh;
class GameCamera;

class SPARKLE_ENGINE_API Scene final
{
  public:
	// ========================================================================
	// Lifecycle
	// ========================================================================

	Scene();
	~Scene() noexcept;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	// ========================================================================
	// Camera
	// ========================================================================

	/// Returns the scene's camera.
	[[nodiscard]] GameCamera& GetCamera() noexcept;
	[[nodiscard]] const GameCamera& GetCamera() const noexcept;

	// ========================================================================
	// Configuration
	// ========================================================================

	/// Configuration for procedural primitive spawning.
	struct PrimitiveConfig
	{
		MeshFactory::Shape shape = MeshFactory::Shape::Box;    ///< Primitive type to spawn
		std::uint32_t count = 500;                             ///< Number of primitives
		DirectX::XMFLOAT3 center = {0.0f, 0.0f, 50.0f};        ///< Center of spawn volume
		DirectX::XMFLOAT3 extents = {100.0f, 100.0f, 100.0f};  ///< Half-extents of spawn volume
		std::uint32_t seed = 1337;                             ///< Random seed for positions
	};

	/// Updates primitive configuration and rebuilds geometry immediately.
	/// Note: Call UploadMeshes() after this to send data to GPU.
	void SetPrimitiveConfig(const PrimitiveConfig& config);

	/// Convenience method to update only shape and count.
	/// Note: Call UploadMeshes() after this to send data to GPU.
	void SetPrimitives(MeshFactory::Shape shape, std::uint32_t count);

	// ========================================================================
	// Accessors
	// ========================================================================

	[[nodiscard]] const PrimitiveConfig& GetPrimitiveConfig() const noexcept { return m_primitiveConfig; }
	[[nodiscard]] MeshFactory::Shape GetCurrentShape() const noexcept { return m_primitiveConfig.shape; }
	[[nodiscard]] std::uint32_t GetCurrentCount() const noexcept { return m_primitiveConfig.count; }
	[[nodiscard]] const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const noexcept;
	[[nodiscard]] bool HasMeshes() const noexcept;

	/// Returns the mesh factory for external GPU upload.
	[[nodiscard]] MeshFactory& GetMeshFactory() noexcept { return *m_meshFactory; }
	[[nodiscard]] const MeshFactory& GetMeshFactory() const noexcept { return *m_meshFactory; }

  private:
	void RebuildGeometry();

	// ------------------------------------------------------------------------
	// Owned Objects
	// ------------------------------------------------------------------------

	std::unique_ptr<GameCamera> m_camera;
	std::unique_ptr<MeshFactory> m_meshFactory;

	// ------------------------------------------------------------------------
	// State
	// ------------------------------------------------------------------------

	PrimitiveConfig m_primitiveConfig;
};
