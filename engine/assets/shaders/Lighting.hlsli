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
		return float3(0.02f, 0.02f, 0.02f);
	}

}  // namespace Lighting