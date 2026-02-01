#include "PCH.h"
#include "Scene.h"
#include "Scene/Mesh.h"
#include "Camera/GameCamera.h"

Scene::Scene() : m_camera(std::make_unique<GameCamera>()), m_meshFactory(std::make_unique<MeshFactory>())
{
	RebuildGeometry();
}

Scene::~Scene() noexcept = default;

GameCamera& Scene::GetCamera() noexcept
{
	return *m_camera;
}

const GameCamera& Scene::GetCamera() const noexcept
{
	return *m_camera;
}

void Scene::SetPrimitiveConfig(const PrimitiveConfig& config)
{
	m_primitiveConfig = config;
	RebuildGeometry();
}

void Scene::SetPrimitives(MeshFactory::Shape shape, std::uint32_t count)
{
	m_primitiveConfig.shape = shape;
	m_primitiveConfig.count = count;
	// Spawn at world origin with default extents
	m_primitiveConfig.center = {0.0f, 0.0f, 0.0f};

	RebuildGeometry();
}

void Scene::RebuildGeometry()
{
	if (!m_meshFactory)
		return;

	// Rebuild meshes (CPU-side geometry only, no GPU upload)
	m_meshFactory->Clear();
	m_meshFactory->AppendShapes(
	    m_primitiveConfig.shape,
	    m_primitiveConfig.count,
	    m_primitiveConfig.center,
	    m_primitiveConfig.extents,
	    m_primitiveConfig.seed);

	// Note: GPU upload must be done externally via MeshFactory::Upload()
	// This keeps Scene decoupled from RHI implementation.
}

const std::vector<std::unique_ptr<Mesh>>& Scene::GetMeshes() const noexcept
{
	static const std::vector<std::unique_ptr<Mesh>> kEmpty;
	if (!m_meshFactory)
		return kEmpty;
	return m_meshFactory->GetMeshes();
}

bool Scene::HasMeshes() const noexcept
{
	return m_meshFactory && !m_meshFactory->GetMeshes().empty();
}
