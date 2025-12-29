#pragma once

#include "Common/Constants.hlsli"
#include "Common/Math.hlsli"
#include "BRDF/Config.hlsli"
#include "BRDF/ShadingData.hlsli"

// =============================================================================
// Subsurface Scattering Approximation
// =============================================================================
// Models light that penetrates the surface, scatters through the material,
// and exits at a different point. Used for skin, wax, foliage, marble, etc.
// These are screen-space approximations, not full SSS simulation.
// =============================================================================

namespace BRDF
{
	namespace Subsurface
	{
		// -------------------------------------------------------------------------
		// Wrap Lighting
		// -------------------------------------------------------------------------
		// Source: GPU Gems 1, Chapter 16 - "Real-Time Approximations to Subsurface"
		//
		// Simple technique that allows light to "wrap" around the terminator.
		// The wrap parameter controls how much light bleeds into shadow regions.
		// Good for simple organic materials and vegetation.
		float3 WrapLighting(float3 albedo, float3 subsurfaceColor, float NoL, float wrap)
		{
			const float wrappedNoL = (NoL + wrap) / (1.0f + wrap);
			const float scatterWidth = saturate(wrappedNoL);
			return albedo * subsurfaceColor * scatterWidth * INV_PI;
		}

		// -------------------------------------------------------------------------
		// Disney Subsurface Approximation
		// -------------------------------------------------------------------------
		// Source: Burley, 2015 - "Extending the Disney BRDF to a BSDF"
		// https://blog.selfshadow.com/publications/s2015-shading-course/burley/s2015_pbs_disney_bsdf_notes.pdf
		//
		// Approximates subsurface scattering as a modified diffuse lobe.
		// Based on Hanrahan-Krueger model but simplified for real-time.
		// Produces characteristic soft lighting and view-dependent glow.
		float3 Disney(float3 albedo, float3 subsurfaceColor, float roughness, float NoV, float NoL, float LoH)
		{
			const float FL = Pow5(1.0f - NoL);
			const float FV = Pow5(1.0f - NoV);

			const float Fss90 = roughness * LoH * LoH;
			const float Fss = lerp(1.0f, Fss90, FL) * lerp(1.0f, Fss90, FV);
			const float ss = 1.25f * (Fss * (1.0f / (NoL + NoV + EPSILON) - 0.5f) + 0.5f);

			return albedo * subsurfaceColor * INV_PI * ss;
		}

		// -------------------------------------------------------------------------
		// Dispatcher
		// -------------------------------------------------------------------------

		float3 EvaluateDirect(float3 albedo, float3 subsurfaceColor, float roughness, float subsurfaceStrength, ShadingData sd)
		{
#if BRDF_SUBSURFACE_MODEL == BRDF_SUBSURFACE_NONE
			return float3(0.0f, 0.0f, 0.0f);
#elif BRDF_SUBSURFACE_MODEL == BRDF_SUBSURFACE_WRAP
			return WrapLighting(albedo, subsurfaceColor, sd.NoL, subsurfaceStrength);
#elif BRDF_SUBSURFACE_MODEL == BRDF_SUBSURFACE_DISNEY
			return Disney(albedo, subsurfaceColor, roughness, sd.NoV, sd.NoL, sd.LoH) * subsurfaceStrength;
#else
			return float3(0.0f, 0.0f, 0.0f);
#endif
		}

	}  // namespace Subsurface
}  // namespace BRDF
