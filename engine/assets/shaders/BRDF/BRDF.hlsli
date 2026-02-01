#pragma once

// =============================================================================
// BRDF Library - Main Entry Point
// =============================================================================
// Complete PBR shading for direct (analytical non-area lights) and indirect (IBL) paths.
//
// USAGE:
//   1. Optionally define model overrides BEFORE including this file:
//        #define BRDF_DIFFUSE_MODEL BRDF_DIFFUSE_BURLEY
//        #define BRDF_SUBSURFACE_MODEL BRDF_SUBSURFACE_DISNEY
//
//   2. Include this file:
//        #include "BRDF/BRDF.hlsli"
//
//   3. Use high-level API:
//        BRDF::Direct::Evaluate(...)   - For analytical lights
//        BRDF::Indirect::Evaluate(...) - For prefiltered IBL
//
// See BRDF/Config.hlsli for all available model options and their descriptions.
// =============================================================================


// BRDF Components
#include "brdf/shadingdata.hlsli"
#include "brdf/fresnel.hlsli"
#include "brdf/distribution.hlsli"
#include "brdf/geometry.hlsli"
#include "brdf/diffuse.hlsli"
#include "brdf/subsurface.hlsli"
#include "brdf/specular.hlsli"
#include "brdf/occlusion.hlsli"

// =============================================================================
// High-Level Evaluation API
// =============================================================================

namespace BRDF
{
	// =========================================================================
	// Direct Lighting (Analytical Light Sources)
	// =========================================================================
	// Evaluates the full BRDF for a single light. Call once per light.
	namespace Direct
	{
		void Evaluate(
		    ShadingData sd,
		    float3 albedo,
		    float roughness,
		    float metallic,
		    float3 F0,
		    float3 subsurfaceColor,
		    float subsurfaceStrength,
		    out float3 outDiffuse,
		    out float3 outSpecular,
		    out float3 outSubsurface)
		{
			// Compute Fresnel once - reused for specular and energy conservation
			const float3 F = Fresnel::EvaluateDirect(sd.VoH, F0);

			// Specular: microfacet BRDF (D * F * G)
			outSpecular = Specular::EvaluateDirect(sd, roughness, F);

			// Diffuse: energy-conserving (1 - F) for dielectrics
			const float3 kD = (1.0f - F) * (1.0f - metallic);
			outDiffuse = Diffuse::EvaluateDirect(albedo, roughness, sd) * kD;

			// Subsurface: additional scattering term (dielectrics only)
			outSubsurface = Subsurface::EvaluateDirect(albedo, subsurfaceColor, roughness, subsurfaceStrength, sd) * (1.0f - metallic);
		}
	}  // namespace Direct

	// =========================================================================
	// Indirect Lighting (Image-Based Lighting / Environment Maps)
	// =========================================================================
	// For prefiltered IBL using the split-sum approximation:
	//   - irradiance: Diffuse irradiance from convolved environment map
	//   - prefilteredEnv: Specular radiance from mip-mapped environment map
	//                     (sample at roughness-based mip level)

	namespace Indirect
	{
		void Evaluate(
		    float NoV,
		    float3 albedo,
		    float roughness,
		    float metallic,
		    float3 F0,
		    float3 irradiance,
		    float3 prefilteredEnv,
		    float ambientOcclusion,
		    out float3 outDiffuse,
		    out float3 outSpecular)
		{
			// Fresnel for indirect uses NoV (no half-vector available)
			const float3 F = Fresnel::EvaluateIndirect(NoV, F0, roughness);

			// Specular IBL: prefiltered environment * BRDF integration
			outSpecular = Specular::EvaluateIndirect(NoV, F0, roughness, prefilteredEnv);

			// Diffuse IBL: irradiance * albedo, energy-conserved
			const float3 kD = (1.0f - F) * (1.0f - metallic);
			outDiffuse = Diffuse::EvaluateIndirect(albedo) * irradiance * kD;

			// Apply occlusion
			outDiffuse *= Occlusion::MultibounceAO(ambientOcclusion, albedo);
			outSpecular *= Occlusion::SpecularOcclusion(NoV, ambientOcclusion, roughness);
		}
	}  // namespace Indirect

}  // namespace BRDF
