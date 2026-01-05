// ============================================================================
// AssetSource.h
// Identifies the origin of engine assets (Project vs Engine).
// ----------------------------------------------------------------------------
// USAGE:
//   auto path = GAssetSystem.GetShaderPath(AssetSource::Engine);
//   auto name = GetAssetSourceName(AssetSource::Project);  // "Project"
//
// DESIGN:
//   - Project assets can override engine defaults
//   - AssetSource::Any checks Project first, then Engine
// ============================================================================
#pragma once

#include <cstdint>
#include <string_view>

enum class AssetSource : uint8_t
{
	Any,      // Check Project first, then Engine (default)
	Project,  // Game assets only
	Engine,   // Built-in engine assets only

	Count
};

[[nodiscard]] constexpr std::string_view GetAssetSourceName(AssetSource source) noexcept
{
	switch (source)
	{
		case AssetSource::Any:
			return "Any";
		case AssetSource::Project:
			return "Project";
		case AssetSource::Engine:
			return "Engine";
		case AssetSource::Count:
		default:
			return "Unknown";
	}
}
