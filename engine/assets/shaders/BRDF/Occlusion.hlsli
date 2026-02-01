#pragma once

#include "brdf/config.hlsli"

// =============================================================================
// Ambient & Specular Occlusion
// =============================================================================
// Techniques to improve indirect lighting by accounting for local occlusion.
// =============================================================================

namespace BRDF
{
	namespace Occlusion
	{
		// =========================================================================
		// Multibounce Ambient Occlusion
		// =========================================================================
		// Standard AO assumes light is fully absorbed after being blocked once.
		// In reality, light bounces multiple times, so AO is too dark, especially
		// for colored surfaces where inter-reflections contribute significant light.

		// -------------------------------------------------------------------------
		// Jimenez Multibounce AO
		// -------------------------------------------------------------------------
		// Source: Jimenez et al., 2016 - "Practical Realtime Strategies for Accurate Indirect Occlusion"
		// https://www.activision.com/cdn/research/Practical_Real_Time_Strategies_for_Accurate_Indirect_Occlusion_NEW%20VERSION_COLOR.pdf
		//
		// Approximates energy gained from multiple bounces using surface albedo.
		// Brighter surfaces recover more energy from inter-reflections.
		float3 MultibounceAO_Jimenez(float ao, float3 albedo)
		{
			const float3 a = 2.0404f * albedo - 0.3324f;
			const float3 b = -4.7951f * albedo + 0.6417f;
			const float3 c = 2.7552f * albedo + 0.6903f;
			return max(ao.xxx, ((ao * a + b) * ao + c) * ao);
		}

		// Dispatcher
		float3 MultibounceAO(float ao, float3 albedo)
		{
#if BRDF_MULTIBOUNCE_AO_MODEL == BRDF_MULTIBOUNCE_AO_JIMENEZ
			return MultibounceAO_Jimenez(ao, albedo);
#else
			return ao.xxx;
#endif
		}

		// =========================================================================
		// Specular Occlusion
		// =========================================================================
		// Diffuse AO doesn't apply well to specular because specular depends on
		// the reflection direction, not the hemisphere. SO approximates how much
		// indirect specular is blocked based on view angle and roughness.

		// -------------------------------------------------------------------------
		// Lagarde Specular Occlusion
		// -------------------------------------------------------------------------
		// Source: Lagarde & de Rousiers, 2014 - "Moving Frostbite to PBR"
		// https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
		//
		// Simple heuristic: specular sees less occlusion at grazing angles
		// because the reflection direction points away from occluders.
		float SpecularOcclusion_Lagarde(float NoV, float ao)
		{
			return saturate(pow(NoV + ao, 2.0f) - 1.0f + ao);
		}

		// -------------------------------------------------------------------------
		// Lagarde Specular Occlusion (Roughness-Aware)
		// -------------------------------------------------------------------------
		// Smoother surfaces have sharper, more localized reflections that are
		// more affected by nearby occluders. Rough surfaces blur the reflection
		// over a wider cone, averaging out occlusion.
		float SpecularOcclusion_LagardeApprox(float NoV, float ao, float roughness)
		{
			return saturate(pow(NoV + ao, exp2(-16.0f * roughness - 1.0f)) - 1.0f + ao);
		}

		// -------------------------------------------------------------------------
		// GTAO Specular Occlusion
		// -------------------------------------------------------------------------
		// Source: Jimenez et al., 2016 - "Practical Realtime Strategies for Accurate Indirect Occlusion"
		//
		// Simpler roughness-aware variant using roughness² as exponent.
		float SpecularOcclusion_GTAO(float NoV, float ao, float roughness)
		{
			return saturate(pow(NoV + ao, roughness * roughness) - 1.0f + ao);
		}

		// Dispatcher
		float SpecularOcclusion(float NoV, float ao, float roughness)
		{
#if BRDF_SO_MODEL == BRDF_SO_NONE
			return 1.0f;
#elif BRDF_SO_MODEL == BRDF_SO_LAGARDE
			return SpecularOcclusion_Lagarde(NoV, ao);
#elif BRDF_SO_MODEL == BRDF_SO_LAGARDE_APPROX
			return SpecularOcclusion_LagardeApprox(NoV, ao, roughness);
#else
			return SpecularOcclusion_GTAO(NoV, ao, roughness);
#endif
		}

	}  // namespace Occlusion
}  // namespace BRDF
