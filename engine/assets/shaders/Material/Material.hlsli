#pragma once

#include "Resources/ConstantBuffers.hlsli"

#include "Geometry/PixelInput.hlsli"
#include "Geometry/Transforms.hlsli"

// =============================================================================
// Material System
// =============================================================================
// Material properties and texture sampling

// -----------------------------------------------------------------------------
// Texture Bindings
// -----------------------------------------------------------------------------

Texture2D TextureBaseColor : register(t0);

sampler SamplerLinearWrap : register(s0);

// -----------------------------------------------------------------------------
// Material Namespace
// -----------------------------------------------------------------------------

namespace Material
{
	// We store Dielectric F0 for precision and remap to actual desired F0 dielectric range
	float RemapDielectricF0(float EncodedF0)
	{
		// Remap from [0.0, 1.0] to [0.00, 0.08]
		return saturate(EncodedF0) * 0.08f;
	}

	// -------------------------------------------------------------------------
	// Material Properties Structure
	// -------------------------------------------------------------------------
	struct Properties
	{
		// Core PBR parameters
		float3 BaseColor;        // Albedo for dielectrics, reflectance for metals
		float3 NormalTangent;    // Normal in tangent space (from normal map)
		float3 NormalWorld;      // Normal in world space (after normal mapping)
		float Roughness;         // Perceptual roughness [0=smooth, 1=rough]
		float Metallic;          // Metalness [0=dielectric, 1=metal]
		float DielectricF0;      // F0 reflectance for dielectrics 0.0 - 1.0 -> remapped[0.00, 0.08]
		float AmbientOcclusion;  // Baked occlusion [0=occluded, 1=exposed]

		// Subsurface scattering
		float3 SubsurfaceColor;    // Color tint for subsurface scattering
		float SubsurfaceStrength;  // Intensity of SSS effect [0=none, 1=full]

		// Emission
		float3 Emissive;  // Self-illumination (HDR, can exceed 1.0)
	};

	// -------------------------------------------------------------------------
	// Default Material
	// -------------------------------------------------------------------------
	Properties MakeDefault()
	{
		Properties props;
		props.BaseColor = float3(1.0f, 1.0f, 1.0f);
		props.NormalTangent = float3(0.0f, 0.0f, 1.0f);
		props.NormalWorld = float3(0.0f, 0.0f, 1.0f);
		props.Roughness = 1.0f;
		props.Metallic = 0.0f;
		props.DielectricF0 = 0.5f;
		props.AmbientOcclusion = 1.0f;
		props.SubsurfaceColor = float3(0.0f, 0.0f, 0.0f);
		props.SubsurfaceStrength = 0.0f;
		props.Emissive = float3(0.0f, 0.0f, 0.0f);
		return props;
	}

	// -------------------------------------------------------------------------
	// Individual Texture Samplers
	// -------------------------------------------------------------------------

	float3 SampleBaseColor(float2 UV)
	{
		return TextureBaseColor.SampleLevel(SamplerLinearWrap, UV, 0.0f).xyz;
	}

	float3 SampleNormalTangent(float2 UV)
	{
		return float3(0.0f, 0.0f, 1.0f);
	}

	float SampleRoughness(float2 UV)
	{
		return 1.0f;
	}

	float3 SampleEmissive(float2 UV)
	{
		return float3(0.0f, 0.0f, 0.0f);
	}

	float SampleMetallic(float2 UV)
	{
		return 0.0f;
	}

	float SampleDielectricF0(float2 UV)
	{
		return RemapDielectricF0(0.5f);
	}

	float SampleAmbientOcclusion(float2 UV)
	{
		return 1.0f;
	}

	float3 SampleSubsurfaceColor(float2 UV)
	{
		return float3(0.0f, 0.0f, 0.0f);
	}

	float SampleSubsurfaceStrength(float2 UV)
	{
		return 0.0f;  // Disabled by default
	}

	// -------------------------------------------------------------------------
	// Full Material Sampling
	// -------------------------------------------------------------------------

	Properties Sample(PS::Input Input)
	{
		Properties props = MakeDefault();
		props.BaseColor = SampleBaseColor(Input.TexCoord);
		props.NormalTangent = SampleNormalTangent(Input.TexCoord);
		props.NormalWorld = TransformNormalToWorld(props.NormalTangent, Input.NormalWorld, Input.TangentWorld, Input.BitangentWorld);
		props.Roughness = SampleRoughness(Input.TexCoord);
		props.Metallic = SampleMetallic(Input.TexCoord);
		props.DielectricF0 = SampleDielectricF0(Input.TexCoord);
		props.AmbientOcclusion = SampleAmbientOcclusion(Input.TexCoord);
		props.SubsurfaceColor = SampleSubsurfaceColor(Input.TexCoord);
		props.SubsurfaceStrength = SampleSubsurfaceStrength(Input.TexCoord);
		props.Emissive = SampleEmissive(Input.TexCoord);
		return props;
	}
}  // namespace Material
