#pragma once

#include <cstdint>
#include <string_view>

// =============================================================================
// AssetSource: Identifies Where an Asset Originates
// =============================================================================
//
// Assets come from two locations:
//   - Project: Game-specific assets that can override engine defaults
//   - Engine:  Built-in engine assets (shaders, default textures, etc.)
//
// When using AssetSource::Any, Project assets take precedence over Engine.
//

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
