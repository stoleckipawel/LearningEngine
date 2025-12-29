#pragma once

// =============================================================================
// BRDF Common Types
// =============================================================================
// Shared structures and utilities used across all BRDF components.
// =============================================================================

namespace BRDF
{
	// =========================================================================
	// Shading Data
	// =========================================================================
	// Pre-computed dot products for BRDF evaluation. Compute once per
	// light/view pair and reuse across all BRDF terms.

	struct ShadingData
	{
		float3 N;  // Surface normal (normalized)
		float3 V;  // View direction, surface to camera (normalized)
		float3 L;  // Light direction, surface to light (normalized)
		float3 H;  // Half vector between V and L (normalized)

		float NoL;  // saturate(dot(N, L)) - diffuse/specular falloff
		float NoV;  // saturate(dot(N, V)) - Fresnel/geometry term
		float NoH;  // saturate(dot(N, H)) - specular highlight peak
		float VoH;  // saturate(dot(V, H)) - Fresnel term input
		float LoH;  // saturate(dot(L, H)) - same as VoH by symmetry
	};

	// Compute all shading vectors and dot products from surface normal,
	// view direction, and light direction.
	ShadingData ComputeShadingData(float3 N, float3 V, float3 L)
	{
		ShadingData sd;
		sd.N = normalize(N);
		sd.V = normalize(V);
		sd.L = normalize(L);
		sd.H = normalize(sd.V + sd.L);

		sd.NoL = saturate(dot(sd.N, sd.L));
		sd.NoV = saturate(dot(sd.N, sd.V));
		sd.NoH = saturate(dot(sd.N, sd.H));
		sd.VoH = saturate(dot(sd.V, sd.H));
		sd.LoH = saturate(dot(sd.L, sd.H));

		return sd;
	}

}  // namespace BRDF
