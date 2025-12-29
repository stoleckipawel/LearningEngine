#pragma once

// =============================================================================
// BRDF Configuration
// =============================================================================
// Define BRDF model selections BEFORE including any BRDF headers to override
// defaults. Each option is documented with its trade-offs.
//
// Example usage in your shader:
//   #define BRDF_DIFFUSE_MODEL BRDF_DIFFUSE_BURLEY
//   #include "BRDF/brdf.hlsli"
// =============================================================================

// -----------------------------------------------------------------------------
// Distribution (D) - Normal Distribution Function
// -----------------------------------------------------------------------------
// Controls the shape of specular highlights based on microfacet orientation.

#define BRDF_DISTRIBUTION_GGX 1          // Industry standard, good balance (default)
#define BRDF_DISTRIBUTION_BECKMANN 2     // Classic, slightly sharper falloff
#define BRDF_DISTRIBUTION_BLINN_PHONG 3  // Legacy, cheaper but less realistic

#ifndef BRDF_DISTRIBUTION_MODEL
	#define BRDF_DISTRIBUTION_MODEL BRDF_DISTRIBUTION_GGX
#endif

// -----------------------------------------------------------------------------
// Geometry (G) - Masking-Shadowing Function
// -----------------------------------------------------------------------------
// Models self-occlusion of microfacets. Correlated variants are more accurate.

#define BRDF_GEOMETRY_SMITH_GGX 1                  // Schlick-GGX approximation
#define BRDF_GEOMETRY_SMITH_GGX_CORRELATED 2       // Height-correlated, most accurate (default)
#define BRDF_GEOMETRY_SMITH_GGX_CORRELATED_FAST 3  // Approximation of correlated, faster
#define BRDF_GEOMETRY_KELEMEN 4                    // Simplified, for clear coats
#define BRDF_GEOMETRY_NEUMANN 5                    // Legacy visibility term

#ifndef BRDF_GEOMETRY_MODEL
	#define BRDF_GEOMETRY_MODEL BRDF_GEOMETRY_SMITH_GGX_CORRELATED
#endif

// -----------------------------------------------------------------------------
// Fresnel (F)
// -----------------------------------------------------------------------------
// Reflectance varies with view angle - more reflection at grazing angles.

#define BRDF_FRESNEL_SCHLICK 1            // Classic Schlick approximation -> direct
#define BRDF_FRESNEL_SCHLICK_ROUGHNESS 2  // Roughness-aware, better for IBL -> indirect

// We currently only support 1 Fresnel for direct & indirect therefore no selection

// -----------------------------------------------------------------------------
// Diffuse Model
// -----------------------------------------------------------------------------
// How light scatters within & across the surface.

#define BRDF_DIFFUSE_LAMBERT 1     // Classic, energy-conserving, cheap (default)
#define BRDF_DIFFUSE_BURLEY 2      // Disney diffuse, roughness-aware retro-reflection
#define BRDF_DIFFUSE_OREN_NAYAR 3  // Rough surface model, view-dependent
#define BRDF_DIFFUSE_CHAN 4        // COD:MW variant, subsurface-aware

#ifndef BRDF_DIFFUSE_MODEL
	#define BRDF_DIFFUSE_MODEL BRDF_DIFFUSE_BURLEY
#endif

// -----------------------------------------------------------------------------
// Subsurface Model
// -----------------------------------------------------------------------------
// Approximates light scattering beneath the surface (skin, wax, foliage).

#define BRDF_SUBSURFACE_NONE 0    // Disabled (default)
#define BRDF_SUBSURFACE_WRAP 1    // Simple wrap lighting, cheap
#define BRDF_SUBSURFACE_DISNEY 2  // Disney BSDF approximation, higher quality

#ifndef BRDF_SUBSURFACE_MODEL
	#define BRDF_SUBSURFACE_MODEL BRDF_SUBSURFACE_WRAP
#endif

// -----------------------------------------------------------------------------
// Specular Occlusion (SO)
// -----------------------------------------------------------------------------
// How much indirect specular is blocked by nearby geometry.

#define BRDF_SO_NONE 0            // Disabled
#define BRDF_SO_LAGARDE 1         // NoV + AO heuristic, cheap
#define BRDF_SO_LAGARDE_APPROX 2  // Roughness-aware, better quality (default)

#ifndef BRDF_SO_MODEL
	#define BRDF_SO_MODEL BRDF_SO_NONE
#endif

// -----------------------------------------------------------------------------
// Multibounce Ambient Occlusion
// -----------------------------------------------------------------------------
// Corrects for energy loss in single-bounce AO using surface color.

#define BRDF_MULTIBOUNCE_AO_NONE 0     // Use raw AO
#define BRDF_MULTIBOUNCE_AO_JIMENEZ 1  // Jimenez heuristic (default)

#ifndef BRDF_MULTIBOUNCE_AO_MODEL
	#define BRDF_MULTIBOUNCE_AO_MODEL BRDF_MULTIBOUNCE_AO_JIMENEZ
#endif
