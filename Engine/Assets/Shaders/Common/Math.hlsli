#pragma once

#include "Common/Constants.hlsli"

// =============================================================================
// Math Utility Functions
// =============================================================================

// Fast power of 5
float Pow5(float x)
{
	const float x2 = x * x;
	return x2 * x2 * x;
}

// Safe normalize (avoids NaN for zero vectors)
float3 SafeNormalize(float3 v)
{
	const float len = length(v);
	return len > EPSILON ? v / len : float3(0.0f, 0.0f, 1.0f);
}

// Remap value from one range to another
float Remap(float value, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}
