
#pragma once

#include <filesystem>
#include <cstdlib>

// AssetPathResolver: Utility for resolving asset file paths for engine and samples.

// AssetType: Used to categorize assets for directory resolution.
enum class AssetType {
    Shader,   // Shader source or binary
    Texture,  // Texture image
    Mesh,     // Mesh geometry
    Other     // Any other asset type
};

// Returns the subdirectory name for a given asset type (e.g. "shaders", "textures").
inline std::filesystem::path GetAssetSubdir(AssetType type)
{
    switch (type) {
        case AssetType::Shader:   return std::filesystem::path("shaders");
        case AssetType::Texture:  return std::filesystem::path("textures");
        case AssetType::Mesh:     return std::filesystem::path("meshes");
        default:                  return std::filesystem::path(); // No subdir for 'Other'
    }
}

// Attempts to resolve an asset path by searching:
//   1. Absolute path (if provided and exists)
//   2. Sample asset directories (SAMPLES_PATH env)
//   3. Engine asset directory (ENGINE_PATH env)
// Returns an absolute path if found, otherwise empty.
inline std::filesystem::path ResolveAssetPath(const std::filesystem::path& inputPath, AssetType type = AssetType::Other)
{
    // If absolute path is provided and exists, use it directly.
    if (inputPath.is_absolute() && std::filesystem::exists(inputPath))
        return inputPath;

    // Build asset subdirectory (e.g. "assets/shaders")
    std::filesystem::path assetSubdir = "assets" / GetAssetSubdir(type);

    // Check sample asset directories (SAMPLES_PATH)
    const char* samplesPathEnv = std::getenv("SAMPLES_PATH");
    if (samplesPathEnv) {
        std::filesystem::path samplesDir(samplesPathEnv);
        if (std::filesystem::exists(samplesDir) && std::filesystem::is_directory(samplesDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(samplesDir)) {
                if (!entry.is_directory())
                    continue; // Only look in directories
                std::filesystem::path sampleAsset = entry.path();
                if (!assetSubdir.empty())
                    sampleAsset /= assetSubdir;
                sampleAsset /= inputPath.filename(); // Use filename only
                if (std::filesystem::exists(sampleAsset))
                    return std::filesystem::absolute(sampleAsset);
            }
        }
    }

    // Check engine asset directory (ENGINE_PATH)
    const char* enginePathEnv = std::getenv("ENGINE_PATH");
    if (enginePathEnv) {
        std::filesystem::path engineAsset(enginePathEnv);
        if (!assetSubdir.empty())
            engineAsset /= assetSubdir;
        engineAsset /= inputPath.filename();
        if (std::filesystem::exists(engineAsset))
            return std::filesystem::absolute(engineAsset);
    }

    // Asset not found: return empty path
    return {};
}
