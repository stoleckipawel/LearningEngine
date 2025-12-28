#pragma once

namespace Lighting
{
	float3 CalculateDirect(float3 NormalWorld)
	{
		const float3 lightDir = normalize(float3(0.0f, 1.0f, 0.0f));

		const float NoL = dot(NormalWorld, lightDir);
		const float NoL_s = saturate(NoL);

		return NoL_s;
	}

	float3 CalculateIndireect()
	{
		float3 ambientColor = float3(0.05f, 0.05f, 0.05f);
		return ambientColor;
	}

}  // namespace Lighting