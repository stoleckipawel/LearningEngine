#include "Common.hlsli"

struct PsInput
{
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

struct PsOutput
{
    float4 Color0 : SV_Target;
};

Texture2D myTexture : register(t0);
sampler textureSampler : register(s0);

void main(in PsInput Input, out PsOutput Output)
{
    float3 texel = myTexture.SampleLevel(textureSampler, Input.TexCoord, 0.0f).xyz;

    //texel *= BaseColor.rgb;
    texel = Input.Color.rgb;
    
    Output.Color0 = float4(texel, 1.0f);
}
