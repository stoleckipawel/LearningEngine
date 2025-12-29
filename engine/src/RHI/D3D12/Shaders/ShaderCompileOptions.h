#pragma once

#include "EngineConfig.h"
#include <filesystem>
#include <vector>
#include <string>

// Shader compilation stage identifiers.
enum class ShaderStage : uint8_t
{
	Vertex,
	Pixel,
	Geometry,
	Hull,
	Domain,
	Compute,
	Count
};

// Returns the DXC target prefix for a shader stage (e.g., "vs" for Vertex).
inline const char* GetShaderStagePrefix(ShaderStage stage)
{
	static constexpr const char* kPrefixes[] = {"vs", "ps", "gs", "hs", "ds", "cs"};
	static_assert(std::size(kPrefixes) == static_cast<size_t>(ShaderStage::Count));
	return kPrefixes[static_cast<size_t>(stage)];
}

// Configuration for a single shader compilation request.
struct ShaderCompileOptions
{
	std::filesystem::path SourcePath;  // Absolute path to the .hlsl file
	std::filesystem::path IncludeDir;  // Root directory for #include resolution
	std::string EntryPoint = "main";   // Entry function name
	ShaderStage Stage = ShaderStage::Pixel;

	// Feature flags
	bool EnableDebugInfo = false;
	bool EnableOptimizations = true;
	bool TreatWarningsAsErrors = true;
	bool StripReflection = true;
	bool StripDebugInfo = true;

	// Additional include directories beyond the primary IncludeDir
	std::vector<std::filesystem::path> AdditionalIncludeDirs;

	// Additional preprocessor defines (format: "NAME" or "NAME=VALUE")
	std::vector<std::string> Defines;

	// Builds the shader model target string (e.g., "vs_6_0").
	std::string BuildTargetProfile() const
	{
		std::string profile;
		profile.reserve(8);
		profile += GetShaderStagePrefix(Stage);
		profile += '_';
		profile += std::to_string(EngineSettings::ShaderModelMajor);
		profile += '_';
		profile += std::to_string(EngineSettings::ShaderModelMinor);
		return profile;
	}
};
