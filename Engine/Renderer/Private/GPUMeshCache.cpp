// =============================================================================
// GPUMeshCache.cpp — Lazy GPU mesh upload manager implementation
// =============================================================================

#include "PCH.h"
#include "GPUMeshCache.h"

#include "D3D12Rhi.h"
#include "Scene/Mesh.h"
#include "Log.h"

// =============================================================================
// Construction
// =============================================================================

GPUMeshCache::GPUMeshCache(D3D12Rhi& rhi) noexcept
    : m_rhi(&rhi)
{
}

// =============================================================================
// Cache Operations
// =============================================================================

GPUMesh* GPUMeshCache::GetOrUpload(const Mesh& cpuMesh)
{
	const Mesh* key = &cpuMesh;

	// Check cache first
	auto it = m_cache.find(key);
	if (it != m_cache.end())
	{
		return it->second.get();
	}

	// Upload new GPU mesh
	auto gpuMesh = std::make_unique<GPUMesh>();
	if (!gpuMesh->Upload(*m_rhi, cpuMesh.GetMeshData()))
	{
		LOG_ERROR("[GPUMeshCache] Failed to upload mesh to GPU");
		return nullptr;
	}

	GPUMesh* result = gpuMesh.get();
	m_cache.emplace(key, std::move(gpuMesh));

	return result;
}

void GPUMeshCache::Clear() noexcept
{
	m_cache.clear();
}

// =============================================================================
// Queries
// =============================================================================

bool GPUMeshCache::Contains(const Mesh& cpuMesh) const noexcept
{
	return m_cache.find(&cpuMesh) != m_cache.end();
}
