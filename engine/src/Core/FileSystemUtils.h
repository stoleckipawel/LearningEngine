#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string_view>

// =============================================================================
// FileSystemUtils: Platform-agnostic filesystem utility functions
// =============================================================================
//
// Provides path normalization, directory queries, and marker-based root discovery.
//
// MARKER HIERARCHY:
//   .sparkle         - Workspace root (repository level)
//   .sparkle-engine  - Engine root (engine/ subdirectory)
//   .sparkle-project - Project root (each game/sample project)
//

namespace Engine::FileSystem
{

	// =========================================================================
	// Marker Files
	// =========================================================================

	inline constexpr std::string_view kWorkspaceMarker = ".sparkle";
	inline constexpr std::string_view kEngineMarker = ".sparkle-engine";
	inline constexpr std::string_view kProjectMarker = ".sparkle-project";

	// =========================================================================
	// Path Normalization
	// =========================================================================

	[[nodiscard]] std::filesystem::path NormalizePath(const std::filesystem::path& path);

	// =========================================================================
	// Directory Queries
	// =========================================================================

	[[nodiscard]] std::filesystem::path GetExecutableDirectory();

	// =========================================================================
	// Marker-Based Discovery
	// =========================================================================

	// Walks up directory tree from startDir looking for a marker file.
	[[nodiscard]] std::optional<std::filesystem::path> FindAncestorWithMarker(
	    const std::filesystem::path& startDir,
	    std::string_view markerFileName,
	    uint32_t maxDepth = 32);

	// Discovers workspace root (.sparkle marker).
	[[nodiscard]] std::optional<std::filesystem::path> DiscoverWorkspaceRoot();

	// Discovers engine root (.sparkle-engine marker).
	[[nodiscard]] std::optional<std::filesystem::path> DiscoverEngineRoot();

	// Discovers project root (.sparkle-project marker).
	[[nodiscard]] std::optional<std::filesystem::path> DiscoverProjectRoot();

}  // namespace Engine::FileSystem
