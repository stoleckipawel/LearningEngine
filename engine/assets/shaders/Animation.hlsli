#include "Utilities.hlsli"

float4 Rotate(float4 positionLocal, float speed)
{
	const float rotationAngle = speed * TWO_PI;  // one full rotation per configured period
	const float cosAngle = cos(rotationAngle);
	const float sinAngle = sin(rotationAngle);

	const float3 rotatedPosition = float3(
	    cosAngle * positionLocal.x + sinAngle * positionLocal.z,
	    positionLocal.y,
	    -sinAngle * positionLocal.x + cosAngle * positionLocal.z);

	return float4(rotatedPosition, positionLocal.w);
}

