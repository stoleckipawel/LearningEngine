#include "RootSignature.hlsl"

float3 color : register(b0);
Texture2D<float4> textures[] : register(t0);
sampler texturesSampler : register(s0);

[RootSignature(ROOTSIG)]
void main(
            in float2 uv : TexCoord,
            out float4 OutColor0 : SV_Target
)
{
    //float4 texel = textures[0].Sample(texturesSampler, uv);
    float4 texel = textures[0].Sample(texturesSampler, uv);
    OutColor0 = float4(texel.rgb, 1.0f);
}