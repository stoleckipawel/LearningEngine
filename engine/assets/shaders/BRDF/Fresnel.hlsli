#pragma once

#include "common/math.hlsli"
#include "brdf/config.hlsli"

// =============================================================================
// Fresnel Term (F)
// =============================================================================
// Models how reflectance increases at grazing angles. All materials become
// ~100% reflective at grazing incidence (the "Fresnel effect").
// =============================================================================

namespace BRDF
{
	namespace Fresnel
	{
		// -------------------------------------------------------------------------
		// Schlick Fresnel Approximation
		// -------------------------------------------------------------------------
		// Source: Schlick, 1994 - "An Inexpensive BRDF Model for Physically-based Rendering"
		// https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94.pdf
		//
		// Approximates the full Fresnel equations with a simple power function.
		// F0 is reflectance at normal incidence (0°), approaches 1.0 at 90°.
		float3 Schlick(float VoH, float3 F0)
		{
			return F0 + (1.0f - F0) * Pow5(1.0f - VoH);
		}

		// -------------------------------------------------------------------------
		// Schlick-Roughness Fresnel (for IBL)
		// -------------------------------------------------------------------------
		// Source: Karis, 2013 - Epic Games PBR Notes
		// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
		//
		// Reduces Fresnel rim on rough surfaces where it would appear too bright
		// due to the diffuse nature of rough specular reflections.
		float3 SchlickRoughness(float NoV, float3 F0, float roughness)
		{
			const float3 Fr = max((1.0f - roughness).xxx, F0) - F0;
			return F0 + Fr * Pow5(1.0f - NoV);
		}

		// -------------------------------------------------------------------------
		// Evaluation Entry Points
		// -------------------------------------------------------------------------

		// For direct lighting: uses VoH (half-vector angle)
		float3 EvaluateDirect(float VoH, float3 F0)
		{
			return Schlick(VoH, F0);
		}

		// For indirect/IBL: uses NoV and accounts for roughness
		float3 EvaluateIndirect(float NoV, float3 F0, float roughness)
		{
			return SchlickRoughness(NoV, F0, roughness);
		}

	}  // namespace Fresnel
}  // namespace BRDF
