#pragma once

#include "common/constants.hlsli"
#include "brdf/config.hlsli"

// =============================================================================
// Geometry Term (G) - Masking-Shadowing Function
// =============================================================================
// Models self-occlusion of microfacets: some microfacets are hidden from
// the light (shadowing) or the viewer (masking). Critical for energy
// conservation and correct highlight shapes at grazing angles.
// =============================================================================

namespace BRDF
{
	namespace Geometry
	{
		// -------------------------------------------------------------------------
		// Smith G1 Building Blocks
		// -------------------------------------------------------------------------

		// Source: Heitz, 2014 - "Understanding the Masking-Shadowing Function"
		// https://jcgt.org/published/0003/02/03/
		//
		// Lambda function for exact GGX geometry term. Used to derive G1.
		float Lambda_GGX(float NdotX, float alpha)
		{
			const float a2 = alpha * alpha;
			const float NdotX2 = NdotX * NdotX;
			return (-1.0f + sqrt(1.0f + a2 * (1.0f - NdotX2) / NdotX2)) * 0.5f;
		}

		// Schlick-GGX approximation of Smith G1, remapped for direct lighting.
		float SmithG1_SchlickGGX(float NdotX, float k)
		{
			return NdotX / (NdotX * (1.0f - k) + k);
		}

		// Exact Smith G1 for GGX, derived from Lambda.
		float SmithG1_GGX(float NdotX, float alpha)
		{
			return 1.0f / (1.0f + Lambda_GGX(NdotX, alpha));
		}

		// -------------------------------------------------------------------------
		// Uncorrelated Smith G (G1_light * G1_view)
		// -------------------------------------------------------------------------
		// Treats masking and shadowing as independent events.
		// Simple but slightly over-occludes at grazing angles.

		float Smith_Uncorrelated_SchlickGGX(float NoV, float NoL, float roughness)
		{
			const float r = roughness + 1.0f;
			const float k = (r * r) / 8.0f;
			return SmithG1_SchlickGGX(NoV, k) * SmithG1_SchlickGGX(NoL, k);
		}

		float Smith_Uncorrelated_GGX(float NoV, float NoL, float alpha)
		{
			return SmithG1_GGX(NoV, alpha) * SmithG1_GGX(NoL, alpha);
		}

		// -------------------------------------------------------------------------
		// Height-Correlated Smith G
		// -------------------------------------------------------------------------
		// Source: Heitz, 2014 - "Understanding the Masking-Shadowing Function"
		//
		// Accounts for correlation between masking and shadowing due to shared
		// height field. More accurate, especially for rough surfaces.
		// NOTE: Includes the 1/(4*NoV*NoL) denominator in the return value.
		float Smith_HeightCorrelated_GGX(float NoV, float NoL, float alpha)
		{
			const float a2 = alpha * alpha;
			const float GGXV = NoL * sqrt(NoV * NoV * (1.0f - a2) + a2);
			const float GGXL = NoV * sqrt(NoL * NoL * (1.0f - a2) + a2);
			return 0.5f / (GGXV + GGXL);
		}

		// Fast approximation of height-correlated Smith.
		// Source: Hammon, 2017 - "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
		float Smith_HeightCorrelated_GGX_Fast(float NoV, float NoL, float roughness)
		{
			const float a = roughness;
			const float GGXV = NoL * (NoV * (1.0f - a) + a);
			const float GGXL = NoV * (NoL * (1.0f - a) + a);
			return 0.5f / (GGXV + GGXL);
		}

		// -------------------------------------------------------------------------
		// Alternative Geometry Terms
		// -------------------------------------------------------------------------

		// Source: Kelemen & Szirmay-Kalos, 2001
		// Simplified visibility for clear coat and thin film layers.
		float Kelemen(float VoH)
		{
			return 1.0f / (4.0f * VoH * VoH + EPSILON);
		}

		// Source: Neumann et al., 1999
		// Legacy visibility term, included for completeness.
		float Neumann(float NoV, float NoL)
		{
			return (NoV * NoL) / max(NoV, NoL);
		}

		// -------------------------------------------------------------------------
		// Dispatchers
		// -------------------------------------------------------------------------

		float EvaluateDirect(float NoV, float NoL, float VoH, float roughness, float alpha)
		{
#if BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX
			return Smith_Uncorrelated_SchlickGGX(NoV, NoL, roughness);
#elif BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX_CORRELATED
			return Smith_HeightCorrelated_GGX(NoV, NoL, alpha);
#elif BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX_CORRELATED_FAST
			return Smith_HeightCorrelated_GGX_Fast(NoV, NoL, roughness);
#elif BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_KELEMEN
			return Kelemen(VoH);
#elif BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_NEUMANN
			return Neumann(NoV, NoL);
#else
			return Smith_HeightCorrelated_GGX(NoV, NoL, alpha);
#endif
		}

		// For IBL: uses different k remapping
		float EvaluateIndirect(float NoV, float NoL, float alpha)
		{
#if BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX
			const float k = (alpha * alpha) / 2.0f;
			return SmithG1_SchlickGGX(NoV, k) * SmithG1_SchlickGGX(NoL, k);
#elif BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX_CORRELATED
			return Smith_HeightCorrelated_GGX(NoV, NoL, alpha);
#elif BRDF_GEOMETRY_MODEL == BRDF_GEOMETRY_SMITH_GGX_CORRELATED_FAST
			const float roughness = sqrt(alpha);
			return Smith_HeightCorrelated_GGX_Fast(NoV, NoL, roughness);
#else
			return Smith_HeightCorrelated_GGX(NoV, NoL, alpha);
#endif
		}

	}  // namespace Geometry
}  // namespace BRDF
