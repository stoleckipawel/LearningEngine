#pragma once
#include "Core/PCH.h"

enum class AssetType {
    Shader,
    Texture,
    Mesh,
    Other
};

inline std::filesystem::path GetAssetSubdir(AssetType type)
{
    switch (type) {
        case AssetType::Shader:   return std::filesystem::path("shaders");
        case AssetType::Texture:  return std::filesystem::path("textures");
        case AssetType::Mesh:     return std::filesystem::path("meshes");
        default:                  return std::filesystem::path();
    }
}

inline std::filesystem::path ResolveAssetPath(const std::filesystem::path& inputPath, AssetType type = AssetType::Other)
{
    // 0. Absolute path provided and exists
    if (inputPath.is_absolute() && std::filesystem::exists(inputPath))
        return inputPath;

    std::filesystem::path assetSubdir = "assets" / GetAssetSubdir(type);

    // 1. Check sample asset directories
    const char* samplesPathEnv = std::getenv("SAMPLES_PATH");
    if (samplesPathEnv) {
        std::filesystem::path samplesDir(samplesPathEnv);
        if (std::filesystem::exists(samplesDir) && std::filesystem::is_directory(samplesDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(samplesDir)) {
                if (!entry.is_directory())
                    continue;
                std::filesystem::path sampleAsset = entry.path();
                if (!assetSubdir.empty())
                    sampleAsset /= assetSubdir;
                sampleAsset /= inputPath.filename();
                if (std::filesystem::exists(sampleAsset))
                    return std::filesystem::absolute(sampleAsset);
            }
        }
    }

    // 2. Check engine asset directory
    const char* enginePathEnv = std::getenv("ENGINE_PATH");
    if (enginePathEnv) {
        std::filesystem::path engineAsset(enginePathEnv);
        if (!assetSubdir.empty())
            engineAsset /= assetSubdir;
        engineAsset /= inputPath.filename();
        if (std::filesystem::exists(engineAsset))
            return std::filesystem::absolute(engineAsset);
    }

    // 3. Asset not found
    return {};
}
