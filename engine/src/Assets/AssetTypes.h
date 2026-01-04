#pragma once

#include <cstdint>
#include <string_view>

// =============================================================================
// AssetType: Classification of Engine Assets
// =============================================================================
//
// Each asset type maps to a canonical subdirectory under the asset root.
// This enables:
//   - Automatic path resolution (e.g., "diffuse.png" -> "textures/diffuse.png")
//   - Type-specific loading strategies (shaders compiled differently than textures)
//   - Organized asset folder structure that mirrors the enum
//
// DIRECTORY STRUCTURE:
//   assets/
//   ├── shaders/           <- AssetType::Shader
//   │   └── ShaderSymbols/ <- AssetType::ShaderSymbols (debug PDBs)
//   ├── textures/          <- AssetType::Texture
//   ├── meshes/            <- AssetType::Mesh
//   ├── materials/         <- AssetType::Material
//   ├── scenes/            <- AssetType::Scene
//   ├── audio/             <- AssetType::Audio
//   ├── fonts/             <- AssetType::Font
//
//

enum class AssetType : uint8_t
{
	Shader,         // HLSL source files (.hlsl, .hlsli)
	ShaderSymbols,  // Compiled shader debug symbols (.pdb)
	Texture,        // Image files (.png, .jpg, .dds, etc.)
	Mesh,           // 3D model files (.gltf, .glb, .obj, etc.)
	Material,       // Material definitions (.mat, .json)
	Scene,          // Scene/level files (.scene, .json)
	Audio,          // Sound files (.wav, .ogg, .mp3)
	Font,           // Font files (.ttf, .otf)

	Count
};

// Returns the canonical subdirectory name for a given asset type.
// Used by AssetSystem to construct full filesystem paths.
[[nodiscard]] constexpr std::string_view GetAssetSubdirectory(AssetType type) noexcept
{
	switch (type)
	{
		case AssetType::Shader:
			return "shaders";
		case AssetType::ShaderSymbols:
			return "shaders/shadersymbols";
		case AssetType::Texture:
			return "textures";
		case AssetType::Mesh:
			return "meshes";
		case AssetType::Material:
			return "materials";
		case AssetType::Scene:
			return "scenes";
		case AssetType::Audio:
			return "audio";
		case AssetType::Font:
			return "fonts";
		case AssetType::Count:
		default:
			return {};
	}
}

// Returns a human-readable name for logging and debugging.
[[nodiscard]] constexpr std::string_view GetAssetTypeName(AssetType type) noexcept
{
	switch (type)
	{
		case AssetType::Shader:
			return "Shader";
		case AssetType::ShaderSymbols:
			return "ShaderSymbols";
		case AssetType::Texture:
			return "Texture";
		case AssetType::Mesh:
			return "Mesh";
		case AssetType::Material:
			return "Material";
		case AssetType::Scene:
			return "Scene";
		case AssetType::Audio:
			return "Audio";
		case AssetType::Font:
			return "Font";
		case AssetType::Count:
		default:
			return "Unknown";
	}
}
