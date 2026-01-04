#pragma once

#include "MeshFactory.h"
#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include <vector>

class Mesh;

// Scene manages all renderable objects and scene state.
class Scene final
{
  public:
	[[nodiscard]] static Scene& Get() noexcept;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	void Initialize();
	void Shutdown() noexcept;

	// Configuration for primitive spawning
	struct PrimitiveConfig
	{
		MeshFactory::Shape shape = MeshFactory::Shape::Box;
		std::uint32_t count = 128;
		DirectX::XMFLOAT3 center = {0.0f, 0.0f, 10.0f};
		DirectX::XMFLOAT3 extents = {20.0f, 20.0f, 20.0f};
		std::uint32_t seed = 1337;
	};

	// Called by UI to update primitive configuration. Rebuilds geometry immediately.
	void SetPrimitiveConfig(const PrimitiveConfig& config);

	// Called by UI to update only shape and count (common case).
	void SetPrimitives(MeshFactory::Shape shape, std::uint32_t count);

	// Accessors for current configuration
	[[nodiscard]] const PrimitiveConfig& GetPrimitiveConfig() const noexcept { return m_primitiveConfig; }
	[[nodiscard]] MeshFactory::Shape GetCurrentShape() const noexcept { return m_primitiveConfig.shape; }
	[[nodiscard]] std::uint32_t GetCurrentCount() const noexcept { return m_primitiveConfig.count; }

	// Read-only access to meshes for rendering
	[[nodiscard]] const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const noexcept;
	[[nodiscard]] bool HasMeshes() const noexcept;

  private:
	Scene() = default;
	~Scene() noexcept;

	void RebuildGeometry();

	PrimitiveConfig m_primitiveConfig;
	std::unique_ptr<MeshFactory> m_meshFactory;
	bool m_initialized = false;
};

inline Scene& GScene = Scene::Get();
