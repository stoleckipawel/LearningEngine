#pragma once

#include "resources/constantbuffers.hlsli"
#include "material/material.hlsli"
#include "brdf/brdf.hlsli"

// =============================================================================
// Lighting System
// =============================================================================
// High-level lighting calculations using BRDF::Direct and BRDF::Indirect.
// Bridges the gap between material properties and the BRDF evaluation API.
// =============================================================================

namespace Lighting
{
	// =========================================================================
	// Light Structures
	// =========================================================================

	struct DirectionalLight
	{
		float3 Direction;  // Normalized, pointing TO the light source
		float3 Radiance;   // Color * intensity (can be HDR)
	};

	// =========================================================================
	// Placeholder Light (until proper light system is implemented)
	// =========================================================================

	DirectionalLight GetMainLight()
	{
		DirectionalLight light;
		light.Direction = normalize(float3(0.5f, 1.0f, 0.3f));
		light.Radiance = float3(1.0f, 1.0f, 1.0f);
		return light;
	}

	// =========================================================================
	// Direct Lighting
	// =========================================================================
	// Evaluates contribution from analytical light sources (directional, point, spot).

	void CalculateDirect(
	    float3 viewDirWorld,
	    Material::Properties matProps,
	    out float3 outDiffuse,
	    out float3 outSpecular,
	    out float3 outSubsurface)
	{
		// Get light and compute shading vectors
		DirectionalLight mainLight = GetMainLight();
		BRDF::ShadingData sd = BRDF::ComputeShadingData(matProps.NormalWorld, viewDirWorld, mainLight.Direction);

		// Early out if surface faces away from light or camera
		if (sd.NoL <= 0.0f || sd.NoV <= 0.0f)
		{
			outDiffuse = 0.0f;
			outSpecular = 0.0f;
			outSubsurface = 0.0f;
			return;
		}

		// Clamp roughness to avoid division by zero in specular
		const float roughness = max(matProps.Roughness, 0.04f);
		const float metallic = saturate(matProps.Metallic);

		const float3 F0 = lerp(matProps.DielectricF0.xxx, matProps.BaseColor, metallic);

		// Evaluate BRDF
		float3 diffuseBRDF, specularBRDF, subsurfaceBRDF;
		BRDF::Direct::Evaluate(
		    sd,
		    matProps.BaseColor,
		    roughness,
		    metallic,
		    F0,
		    matProps.SubsurfaceColor,
		    matProps.SubsurfaceStrength,
		    diffuseBRDF,
		    specularBRDF,
		    subsurfaceBRDF);

		// Apply light radiance and geometric attenuation
		outDiffuse = diffuseBRDF * mainLight.Radiance * sd.NoL;
		outSpecular = specularBRDF * mainLight.Radiance * sd.NoL;
		outSubsurface = subsurfaceBRDF * mainLight.Radiance;  // SSS ignores NoL
	}

	// =========================================================================
	// Indirect Lighting (IBL)
	// =========================================================================
	// Uses prefiltered environment maps for image-based lighting.
	//
	// Inputs:
	//   irradiance:     Diffuse irradiance (cosine-weighted hemisphere integral)
	//   prefilteredEnv: Specular radiance at reflection direction, mip-selected by roughness

	void CalculateIndirectIBL(
	    float3 viewDirWorld,
	    Material::Properties matProps,
	    float3 irradiance,
	    float3 prefilteredEnv,
	    out float3 outDiffuse,
	    out float3 outSpecular)
	{
		const float3 N = normalize(matProps.NormalWorld);
		const float3 V = normalize(viewDirWorld);
		const float NoV = saturate(dot(N, V));

		const float roughness = max(matProps.Roughness, 0.01f);
		const float metallic = matProps.Metallic;

		const float3 F0 = lerp(matProps.DielectricF0.xxx, matProps.BaseColor, metallic);

		BRDF::Indirect::Evaluate(
		    NoV,
		    matProps.BaseColor,
		    roughness,
		    metallic,
		    F0,
		    irradiance,
		    prefilteredEnv,
		    matProps.AmbientOcclusion,
		    outDiffuse,
		    outSpecular);
	}
}  // namespace Lighting
