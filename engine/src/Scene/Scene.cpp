#include "PCH.h"
#include "Scene.h"
#include "D3D12Rhi.h"
#include "Camera.h"
#include "Scene/Mesh.h"

Scene& Scene::Get() noexcept
{
	static Scene instance;
	return instance;
}

Scene::~Scene() noexcept = default;

void Scene::Initialize()
{
	if (m_initialized)
		return;

	m_meshFactory = std::make_unique<MeshFactory>();

	// Build initial geometry
	RebuildGeometry();

	m_initialized = true;
}

void Scene::Shutdown() noexcept
{
	m_meshFactory.reset();
	m_initialized = false;
}

void Scene::SetPrimitiveConfig(const PrimitiveConfig& config)
{
	m_primitiveConfig = config;
	RebuildGeometry();
}

void Scene::SetPrimitives(MeshFactory::Shape shape, std::uint32_t count)
{
	// Update spawn center based on current camera position
	const DirectX::XMFLOAT3 camPos = GCamera.GetPosition();
	const DirectX::XMFLOAT3 camDir = GCamera.GetDirection();

	m_primitiveConfig.shape = shape;
	m_primitiveConfig.count = count;
	m_primitiveConfig.center = {camPos.x + camDir.x * 10.0f, camPos.y + camDir.y * 10.0f, camPos.z + camDir.z * 10.0f};

	RebuildGeometry();
}

void Scene::RebuildGeometry()
{
	if (!m_meshFactory)
		return;

	// Flush GPU before destroying/recreating geometry
	GD3D12Rhi.Flush();

	m_meshFactory->Rebuild(
	    m_primitiveConfig.shape,
	    m_primitiveConfig.count,
	    m_primitiveConfig.center,
	    m_primitiveConfig.extents,
	    m_primitiveConfig.seed);

	// To Do: remove this rendering code from scene.cpp the scene should be decoupled

	// Execute upload commands
	GD3D12Rhi.CloseCommandListScene();
	GD3D12Rhi.ExecuteCommandList();
	GD3D12Rhi.Flush();

	// Reset for next frame
	GD3D12Rhi.ResetCommandAllocator();
	GD3D12Rhi.ResetCommandList();
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
