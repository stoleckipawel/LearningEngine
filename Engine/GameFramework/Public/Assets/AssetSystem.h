// ============================================================================
// AssetSystem.h
// Unified asset path resolution service with marker-based discovery.
// ----------------------------------------------------------------------------
// USAGE:
//   AssetSystem assetSystem;  // Auto-discovers paths in constructor
//   const auto& shaderDir = assetSystem.GetShaderPath();
//   auto fullPath = assetSystem.ResolvePath(AssetType::Texture, "diffuse.png");
//
// DESIGN:
//   Marker Hierarchy (files that identify directory roles):
//     .sparkle         - Workspace root (repository level)
//     .sparkle-engine  - Engine root (engine/ subdirectory)
//     .sparkle-project - Project root (each game/sample project)
//
//   Discovery walks up from executable/working directory for markers.
//   If workspace root is found, engine is at <workspace>/engine/.
// ============================================================================
#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"

#include "Assets/AssetSource.h"
#include "Assets/AssetTypes.h"

#include <array>
#include <filesystem>
#include <optional>

class SPARKLE_ENGINE_API AssetSystem final
{
  public:
	AssetSystem();
	~AssetSystem();

	AssetSystem(const AssetSystem&) = delete;
	AssetSystem& operator=(const AssetSystem&) = delete;
	AssetSystem(AssetSystem&&) = delete;
	AssetSystem& operator=(AssetSystem&&) = delete;

	// =========================================================================
	// Root Path Accessors
	// =========================================================================

	[[nodiscard]] const std::filesystem::path& GetProjectPath() const noexcept { return m_projectPath; }
	[[nodiscard]] const std::filesystem::path& GetProjectAssetsPath() const noexcept { return m_projectAssetsPath; }
	[[nodiscard]] const std::filesystem::path& GetEnginePath() const noexcept { return m_enginePath; }
	[[nodiscard]] const std::filesystem::path& GetEngineAssetsPath() const noexcept { return m_engineAssetsPath; }

	[[nodiscard]] const std::filesystem::path& GetWorkingDirectory() const noexcept { return m_workingDirectory; }
	[[nodiscard]] const std::filesystem::path& GetExecutableDirectory() const noexcept { return m_executableDirectory; }

	// =========================================================================
	// Path Accessors
	// =========================================================================

	// Returns the directory path for a specific asset type.
	// When source is Any: returns Project path if available, otherwise Engine.
	[[nodiscard]] const std::filesystem::path& GetTypedPath(AssetType type, AssetSource source = AssetSource::Any) const noexcept;

	[[nodiscard]] const std::filesystem::path& GetShaderPath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetShaderSymbolsPath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetTexturePath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetMeshPath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetMaterialPath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetScenePath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetAudioPath(AssetSource source = AssetSource::Any) const noexcept;
	[[nodiscard]] const std::filesystem::path& GetFontPath(AssetSource source = AssetSource::Any) const noexcept;

	// =========================================================================
	// Path Resolution
	// =========================================================================

	// Resolves a virtual path to an absolute physical path.
	// Searches Project first, then Engine. Returns nullopt if not found.
	[[nodiscard]] std::optional<std::filesystem::path> ResolvePath(const std::filesystem::path& virtualPath, AssetType type) const;

	// Resolves a virtual path to an absolute physical path.
	// Fatals if the asset cannot be found. Use when the asset is required.
	[[nodiscard]] std::filesystem::path ResolvePathValidated(const std::filesystem::path& virtualPath, AssetType type) const;

	// =========================================================================
	// Output Paths
	// =========================================================================

	[[nodiscard]] const std::filesystem::path& GetShaderSymbolsOutputPath() const noexcept { return m_shaderSymbolsOutputPath; }

	// =========================================================================
	// Queries
	// =========================================================================

	[[nodiscard]] bool HasProjectAssets() const noexcept { return !m_projectAssetsPath.empty(); }
	[[nodiscard]] bool HasEngineAssets() const noexcept { return !m_engineAssetsPath.empty(); }

  private:
	void DiscoverPaths();
	void InitializeTypedPaths();
	void InitializeOutputPaths();
	void ValidatePaths();

	[[nodiscard]] std::optional<std::filesystem::path> TryResolveIn(
	    const std::filesystem::path& searchDir,
	    const std::filesystem::path& relativePath,
	    AssetType type) const;

	static constexpr size_t kAssetTypeCount = static_cast<size_t>(AssetType::Count);

	// Discovered root paths
	std::filesystem::path m_projectPath;
	std::filesystem::path m_projectAssetsPath;
	std::filesystem::path m_enginePath;
	std::filesystem::path m_engineAssetsPath;
	std::filesystem::path m_workingDirectory;
	std::filesystem::path m_executableDirectory;

	// Cached typed paths for fast lookup
	std::array<std::filesystem::path, kAssetTypeCount> m_projectTypedPaths{};
	std::array<std::filesystem::path, kAssetTypeCount> m_engineTypedPaths{};

	// Output directories
	std::filesystem::path m_shaderSymbolsOutputPath;

	inline static const std::filesystem::path s_emptyPath{};
};
