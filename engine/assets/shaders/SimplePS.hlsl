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
    float3 color = myTexture.SampleLevel(textureSampler, Input.TexCoord, 0.0f).xyz;

    //color *= BaseColor.rgb;
    color *= Input.Color.rgb;
    
    Output.Color0 = float4(color, 1.0f);
}
