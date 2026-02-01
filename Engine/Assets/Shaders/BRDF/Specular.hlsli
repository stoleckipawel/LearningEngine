#pragma once

#include "Common/Constants.hlsli"
#include "BRDF/Config.hlsli"
#include "BRDF/ShadingData.hlsli"
#include "BRDF/Fresnel.hlsli"
#include "BRDF/Distribution.hlsli"
#include "BRDF/Geometry.hlsli"

// =============================================================================
// Specular BRDF (Cook-Torrance Microfacet Model)
// =============================================================================
// Models surface reflection from microfacets. Combines:
//   D = Normal Distribution Function (highlight shape)
//   F = Fresnel (angle-dependent reflectance)
//   G = Geometry/Visibility (self-shadowing)
//
// Cook-Torrance: f_spec = (D * F * G) / (4 * NoV * NoL)
// =============================================================================

namespace BRDF
{
	namespace Specular
	{
		// -------------------------------------------------------------------------
		// Cook-Torrance Combination
		// -------------------------------------------------------------------------
		// Combines D, F, G terms. Note: correlated geometry models already
		// include the 1/(4*NoV*NoL) denominator in their return value.
		float3 CookTorrance(float D, float3 F, float G, float NoV, float NoL)
		{
#if BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX_CORRELATED || BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX_CORRELATED_FAST
			// Correlated G already includes denominator
			return D * F * G;
#else
			const float denom = max(4.0f * NoV * NoL, EPSILON);
			return (D * F * G) / denom;
#endif
		}

		// -------------------------------------------------------------------------
		// Direct Lighting Evaluation
		// -------------------------------------------------------------------------
		// Takes pre-computed Fresnel term F (not F0) to avoid redundant
		// computation when caller also needs F for energy conservation.
		float3 EvaluateDirect(ShadingData sd, float roughness, float3 F)
		{
			const float alpha = roughness * roughness;

			const float D = Distribution::Evaluate(sd.NoH, alpha);
			const float G = Geometry::EvaluateDirect(sd.NoV, sd.NoL, sd.VoH, roughness, alpha);

			return CookTorrance(D, F, G, sd.NoV, sd.NoL);
		}

		// -------------------------------------------------------------------------
		// BRDF Integration Approximation (for IBL Split-Sum)
		// -------------------------------------------------------------------------
		// Source: Karis, 2013 - "Real Shading in Unreal Engine 4"
		// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
		// Source: Lazarov, 2013 - "Getting More Physical in Call of Duty: Black Ops II"
		// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
		//
		// Polynomial fit to the pre-integrated BRDF lookup table.
		// Returns (scale, bias) to apply to F0: specular = prefilteredEnv * (F0 * scale + bias)
		// Only valid for prefiltered IBL using GGX importance sampling.
		float2 ApproximateBRDFIntegration(float NoV, float roughness)
		{
			const float4 c0 = float4(-1.0f, -0.0275f, -0.572f, 0.022f);
			const float4 c1 = float4(1.0f, 0.0425f, 1.04f, -0.04f);
			float4 r = roughness * c0 + c1;
			float a004 = min(r.x * r.x, exp2(-9.28f * NoV)) * r.x + r.y;
			return float2(-1.04f, 1.04f) * a004 + r.zw;
		}

		// -------------------------------------------------------------------------
		// Indirect (IBL) Evaluation
		// -------------------------------------------------------------------------
		// For prefiltered IBL: D is baked into environment mips, G is approximated
		// in the BRDF LUT. We apply the Fresnel term and BRDF integration.
		//
		// prefilteredEnv: Environment map sampled at reflection direction with
		//                 roughness-based mip level
		// brdfLUT: If using texture LUT instead of approximation, sample with (NoV, roughness) -> ToDo: Genenrate LUT
		float3 EvaluateIndirectWithLUT(float NoV, float3 F0, float roughness, float3 prefilteredEnv, float2 brdfLUT)
		{
			const float3 F = Fresnel::EvaluateIndirect(NoV, F0, roughness);
			return prefilteredEnv * (F * brdfLUT.x + brdfLUT.y);
		}

		// Simplified version using polynomial approximation instead of LUT texture
		float3 EvaluateIndirect(float NoV, float3 F0, float roughness, float3 prefilteredEnv)
		{
			const float2 brdf = ApproximateBRDFIntegration(NoV, roughness);
			const float3 F = Fresnel::EvaluateIndirect(NoV, F0, roughness);
			return prefilteredEnv * (F * brdf.x + brdf.y);
		}

	}  // namespace Specular
}  // namespace BRDF
