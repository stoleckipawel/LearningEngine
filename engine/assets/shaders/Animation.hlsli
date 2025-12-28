#include "Utilities.hlsli"

float3 Rotate(float3 value, float3 axis, float offset)
{
	const float theta = offset * TWO_PI;
	const float cosAngle = cos(theta);
	const float sinAngle = sin(theta);
	return value * cosAngle + cross(axis, value) * sinAngle + axis * (dot(axis, value) * (1.0f - cosAngle));
}

// Rotate (preserve w)
float4 Rotate(float4 value, float3 axis, float offset)
{
	float3 r = Rotate(value.xyz, axis, offset);
	return float4(r, value.w);
}
