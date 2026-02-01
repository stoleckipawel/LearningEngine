#pragma once

#include "Common/Constants.hlsli"
#include "BRDF/Config.hlsli"

// =============================================================================
// Distribution Term (D) - Normal Distribution Function
// =============================================================================
// Models the statistical distribution of microfacet orientations on a rough
// surface. Determines the shape and intensity of specular highlights.
// Input: NoH (alignment of surface normal with half-vector)
// =============================================================================

namespace BRDF
{
	namespace Distribution
	{
		// -------------------------------------------------------------------------
		// GGX / Trowbridge-Reitz Distribution
		// -------------------------------------------------------------------------
		// Source: Walter et al., 2007 - "Microfacet Models for Refraction"
		// https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
		//
		// Industry standard NDF with long highlight tails. Matches real materials
		// better than Beckmann, especially at grazing angles.
		float GGX(float NoH, float alpha)
		{
			const float a2 = alpha * alpha;
			const float f = (NoH * NoH) * (a2 - 1.0f) + 1.0f;
			return a2 / (PI * f * f);
		}

		// -------------------------------------------------------------------------
		// Beckmann Distribution
		// -------------------------------------------------------------------------
		// Source: Beckmann & Spizzichino, 1963 - "The Scattering of EM Waves"
		//
		// Classic microfacet distribution, sharper falloff than GGX.
		// Derived from Gaussian height distribution of surface roughness.
		float Beckmann(float NoH, float alpha)
		{
			const float a2 = alpha * alpha;
			const float NoH2 = NoH * NoH;
			const float exponent = (NoH2 - 1.0f) / (a2 * NoH2);
			return exp(exponent) / (PI * a2 * NoH2 * NoH2);
		}

		// -------------------------------------------------------------------------
		// Blinn-Phong Distribution
		// -------------------------------------------------------------------------
		// Source: Blinn, 1977 - "Models of Light Reflection for Computer Graphics"
		//
		// Legacy model, fast but less physically accurate. The exponent n is
		// derived from roughness to maintain energy conservation.
		float BlinnPhong(float NoH, float alpha)
		{
			const float a2 = max(alpha * alpha, EPSILON);
			const float n = 2.0f / a2 - 2.0f;
			return (n + 2.0f) / (2.0f * PI) * pow(max(NoH, 0.0f), n);
		}

		// -------------------------------------------------------------------------
		// Dispatcher
		// -------------------------------------------------------------------------
		float Evaluate(float NoH, float alpha)
		{
#if BRDF_DISTRIBUTION_MODEL == BRDF_DISTRIBUTION_GGX
			return GGX(NoH, alpha);
#elif BRDF_DISTRIBUTION_MODEL == BRDF_DISTRIBUTION_BECKMANN
			return Beckmann(NoH, alpha);
#elif BRDF_DISTRIBUTION_MODEL == BRDF_DISTRIBUTION_BLINN_PHONG
			return BlinnPhong(NoH, alpha);
#else
			return GGX(NoH, alpha);
#endif
		}

	}  // namespace Distribution
}  // namespace BRDF
