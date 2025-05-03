#include "RootSignature.hlsl"

[RootSignature(ROOTSIG)]
float4 main(float3 pos : Position) : SV_Position
{
    return float4(pos.xy, 0.0f, 1.0f);
}