// ============================================================================
// TextureManager.cpp
// ============================================================================

#include "PCH.h"
#include "TextureManager.h"
#include "D3D12Texture.h"
#include "Assets/AssetSystem.h"
#include "D3D12Rhi.h"
#include "D3D12DescriptorHeapManager.h"
#include "Log.h"

#include <format>

TextureManager::TextureManager(const AssetSystem& assetSystem, D3D12Rhi& rhi, D3D12DescriptorHeapManager& descriptorHeapManager) noexcept :
    m_assetSystem(&assetSystem), m_rhi(&rhi), m_descriptorHeapManager(&descriptorHeapManager)
{
	LoadDefaults();
}

TextureManager::~TextureManager() noexcept
{
	UnloadAll();
}

void TextureManager::LoadDefaults()
{
	LoadTexture(TextureId::Checker, "ColorCheckerBoard.png");
	LoadTexture(TextureId::SkyCubemap, "SkyCubemap.png");

	LOG_INFO(std::format("TextureManager: Loaded {} default textures", GetLoadedCount()));
}

void TextureManager::LoadTexture(TextureId id, const std::filesystem::path& relativePath)
{
	const auto index = static_cast<std::size_t>(id);
	if (index >= kTextureCount)
	{
		LOG_ERROR(std::format("TextureManager::LoadTexture: Invalid texture ID {}", index));
		return;
	}

	// Unload existing texture at this slot if present
	if (m_textures[index])
	{
		LOG_DEBUG(std::format("TextureManager: Replacing texture at slot {}", index));
		m_textures[index].reset();
	}

	m_textures[index] = std::make_unique<D3D12Texture>(*m_assetSystem, *m_rhi, relativePath, *m_descriptorHeapManager);

	LOG_DEBUG(std::format("TextureManager: Loaded '{}' at slot {}", relativePath.string(), index));
}

void TextureManager::UnloadTexture(TextureId id) noexcept
{
	const auto index = static_cast<std::size_t>(id);
	if (index < kTextureCount)
	{
		m_textures[index].reset();
	}
}

void TextureManager::UnloadAll() noexcept
{
	for (auto& texture : m_textures)
	{
		texture.reset();
	}
}

D3D12Texture* TextureManager::GetTexture(TextureId id) noexcept
{
	const auto index = static_cast<std::size_t>(id);
	return (index < kTextureCount) ? m_textures[index].get() : nullptr;
}

const D3D12Texture* TextureManager::GetTexture(TextureId id) const noexcept
{
	const auto index = static_cast<std::size_t>(id);
	return (index < kTextureCount) ? m_textures[index].get() : nullptr;
}

bool TextureManager::IsLoaded(TextureId id) const noexcept
{
	const auto index = static_cast<std::size_t>(id);
	return (index < kTextureCount) && (m_textures[index] != nullptr);
}

std::size_t TextureManager::GetLoadedCount() const noexcept
{
	std::size_t count = 0;
	for (const auto& texture : m_textures)
	{
		if (texture)
			++count;
	}
	return count;
}
