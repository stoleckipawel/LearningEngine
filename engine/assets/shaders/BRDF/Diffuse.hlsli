#pragma once

#include "common/constants.hlsli"
#include "common/math.hlsli"
#include "brdf/config.hlsli"
#include "brdf/shadingdata.hlsli"

// =============================================================================
// Diffuse BRDF
// =============================================================================
// Models light that enters the surface, scatters internally, and exits.
// This is the "body" reflection as opposed to the "surface" (specular).
// =============================================================================

namespace BRDF
{
	namespace Diffuse
	{
		// -------------------------------------------------------------------------
		// Lambertian Diffuse
		// -------------------------------------------------------------------------
		// Classic energy-conserving diffuse. Assumes uniform scattering in all
		// directions. Simple but ignores view/roughness effects.
		float3 Lambert(float3 albedo)
		{
			return albedo * INV_PI;
		}

		// -------------------------------------------------------------------------
		// Disney/Burley Diffuse
		// -------------------------------------------------------------------------
		// Source: Burley, 2012 - Disney "Principled BRDF"
		// https://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v2.pdf
		//
		// Adds roughness-dependent retro-reflection: rough surfaces appear
		// brighter at grazing angles due to light bouncing back toward the source.
		float3 Burley(float3 albedo, float roughness, float NoV, float NoL, float LoH)
		{
			const float f90 = 0.5f + 2.0f * roughness * LoH * LoH;
			const float lightScatter = 1.0f + (f90 - 1.0f) * Pow5(1.0f - NoL);
			const float viewScatter = 1.0f + (f90 - 1.0f) * Pow5(1.0f - NoV);
			return albedo * INV_PI * lightScatter * viewScatter;
		}

		// -------------------------------------------------------------------------
		// Oren-Nayar Diffuse
		// -------------------------------------------------------------------------
		// Source: Oren & Nayar, 1994 - "Generalization of Lambert's Reflectance Model"
		// https://www.cs.cornell.edu/~srm/publications/OranNayar.pdf
		//
		// Physically-based rough surface model. Accounts for inter-reflection
		// between surface facets. Good for cloth, concrete, rough plastics.
		float3 OrenNayar(float3 albedo, float roughness, float NoV, float NoL, float3 N, float3 V, float3 L)
		{
			const float sigma2 = roughness * roughness;
			const float A = 1.0f - 0.5f * sigma2 / (sigma2 + 0.33f);
			const float B = 0.45f * sigma2 / (sigma2 + 0.09f);

			const float3 Vtan = normalize(V - N * NoV);
			const float3 Ltan = normalize(L - N * NoL);
			const float cosPhiDiff = max(0.0f, dot(Vtan, Ltan));

			const float sinThetaV = sqrt(max(0.0f, 1.0f - NoV * NoV));
			const float sinThetaL = sqrt(max(0.0f, 1.0f - NoL * NoL));
			const float sinAlpha = max(sinThetaV, sinThetaL);
			const float tanBeta = min(sinThetaV / max(NoV, EPSILON), sinThetaL / max(NoL, EPSILON));

			return albedo * INV_PI * (A + B * cosPhiDiff * sinAlpha * tanBeta);
		}

		// -------------------------------------------------------------------------
		// Chan Diffuse (Call of Duty: Modern Warfare)
		// -------------------------------------------------------------------------
		// Source: Chan, 2020 - "Material Advances in COD:MW"
		// https://www.activision.com/cdn/research/s2020_materials_cod_notes_v2.pdf
		//
		// Blends standard diffuse with a subsurface-like term based on roughness.
		// Designed for skin and organic materials.
		float3 Chan(float3 albedo, float roughness, float NoV, float NoL, float LoH, float NoH)
		{
			const float f90 = 0.5f + 2.0f * roughness * LoH * LoH;

			const float FV = 1.0f + (f90 - 1.0f) * Pow5(1.0f - NoV);
			const float FL = 1.0f + (f90 - 1.0f) * Pow5(1.0f - NoL);

			const float Fd90 = f90;
			const float Fd = lerp(1.0f, Fd90, FL) * lerp(1.0f, Fd90, FV);

			const float Fss90 = roughness * LoH * LoH;
			const float Fss = lerp(1.0f, Fss90, FL) * lerp(1.0f, Fss90, FV);
			const float ss = 1.25f * (Fss * (1.0f / (NoL + NoV) - 0.5f) + 0.5f);

			return albedo * INV_PI * lerp(Fd, ss, saturate(roughness));
		}

		// -------------------------------------------------------------------------
		// Dispatchers
		// -------------------------------------------------------------------------

		float3 EvaluateDirect(float3 albedo, float roughness, ShadingData sd)
		{
#if BRDF_DIFFUSE_MODEL == BRDF_DIFFUSE_LAMBERT
			return Lambert(albedo);
#elif BRDF_DIFFUSE_MODEL == BRDF_DIFFUSE_BURLEY
			return Burley(albedo, roughness, sd.NoV, sd.NoL, sd.LoH);
#elif BRDF_DIFFUSE_MODEL == BRDF_DIFFUSE_OREN_NAYAR
			return OrenNayar(albedo, roughness, sd.NoV, sd.NoL, sd.N, sd.V, sd.L);
#elif BRDF_DIFFUSE_MODEL == BRDF_DIFFUSE_CHAN
			return Chan(albedo, roughness, sd.NoV, sd.NoL, sd.LoH, sd.NoH);
#else
			return Lambert(albedo);
#endif
		}

		// For indirect: always Lambert (diffuse IBL is pre-integrated assuming Lambert)
		float3 EvaluateIndirect(float3 albedo)
		{
			return Lambert(albedo);
		}

	}  // namespace Diffuse
}  // namespace BRDF
