#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

// =============================================================================
// FileSystemUtils: Platform-agnostic filesystem utility functions
// =============================================================================

namespace Engine::FileSystem
{

	// Normalizes a path to a canonical absolute form.
	// - Converts relative paths to absolute
	// - Uses platform-preferred separators
	// - Resolves symlinks and . / .. components
	[[nodiscard]] std::filesystem::path NormalizePath(const std::filesystem::path& path);

	// Returns the directory containing the running executable.
	[[nodiscard]] std::filesystem::path GetExecutableDirectory();

	// Returns the value of an environment variable interpreted as a filesystem path.
	// Empty variables are treated as not present.
	[[nodiscard]] std::optional<std::filesystem::path> TryGetEnvironmentPath(std::string_view variableName);

}  // namespace Engine::FileSystem
