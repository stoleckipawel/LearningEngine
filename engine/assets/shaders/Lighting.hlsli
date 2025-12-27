#pragma once

namespace Lighting
{
    float CalculateDirect(float3 NormalWorld)
    {
        const float3 lightDir = normalize(float3(0.0f, 1.0f, 0.0f));

        const float NoL = dot(NormalWorld, lightDir);
        const float NoL_s = saturate(NoL);
        
        return NoL_s;
    }
}