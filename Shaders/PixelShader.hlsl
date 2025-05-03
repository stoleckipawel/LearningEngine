#include "RootSignature.hlsl"

float3 color : register(b0);

[RootSignature(ROOTSIG)]
float4 main() : SV_Target
{
    return float4(1.0, 0.0, 0.0, 1.0f);
}