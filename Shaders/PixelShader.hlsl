#include "RootSignature.hlsl"

float3 color : register(b0);
Texture2D<float4> textures[] : register(t0);
sampler texturesSampler : register(s0);

[RootSignature(ROOTSIG)]
float4 main() : SV_Target
{
    float3 texel = textures[0].Sample(texturesSampler, float2(0.5f, 0.5f));
    return float4(texel.rgb, 1.0f);
}